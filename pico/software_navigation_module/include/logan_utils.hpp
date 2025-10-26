#ifndef LOGAN_UTILS_HPP
#define LOGAN_UTILS_HPP

#include <pico/time.h>
#include <hardware/gpio.h>
#include <cstdint>

#include "spi_comm.hpp"

class Logan {
public:
  struct TriggerFlags {
    bool edge_rising;
    bool edge_falling;
    bool level_low;
    bool level_high;
  };

  static inline void take_snapshot(const uint8_t *pins, uint8_t (&out)[4])
  {
    uint32_t all = gpio_get_all();
    out[0] = (all >> pins[0]) & 0x1;
    out[1] = (all >> pins[1]) & 0x1;
    out[2] = (all >> pins[2]) & 0x1;
    out[3] = (all >> pins[3]) & 0x1;
  }

  static inline int64_t interval_us_from_rate_nibble(uint8_t rate_nibble)
  {
    size_t rate_hz = SPIComm::rate_from_nibble(rate_nibble);
    if (rate_hz < 1) rate_hz = 1;
    int64_t us = (int64_t)((1000000 + (rate_hz / 2)) / rate_hz);
    if (us <= 0) us = 1;
    return us;
  }

  static inline void start_sampling_timer(int64_t interval_us, repeating_timer_t *timer, repeating_timer_callback_t cb)
  {
    add_repeating_timer_us(-interval_us, cb, nullptr, timer);
  }

  static inline void stop_sampling_timer(repeating_timer_t *timer)
  {
    cancel_repeating_timer(timer);
  }

  static inline uint16_t build_header_word(uint8_t channel_1to4, uint8_t trig_mode, uint8_t samples_nib, uint8_t rate_nib, bool continuous = true)
  {
    uint8_t type_nib = static_cast<uint8_t>(((continuous ? 0x8 : 0x0)) | (channel_1to4 & 0x07));
    return static_cast<uint16_t>(((type_nib   & 0x0F) << 12) |
                                 ((trig_mode  & 0x0F) << 8)  |
                                 ((samples_nib& 0x0F) << 4)  |
                                 ((rate_nib   & 0x0F) << 0));
  }

  static inline TriggerFlags flags_from_mode(uint8_t mode)
  {
    TriggerFlags f{};
    switch (mode)
    {
      case 1: f = {false, false, true,  false}; break;   // Low level
      case 2: f = {false, false, false, true}; break;    // High level
      case 3: f = {true,  false, false, false}; break;   // Rising edge
      case 4: f = {false, true,  false, false}; break;   // Falling edge
      case 5: f = {true,  true,  false, false}; break;   // Either edge
      default: f = {true,  false, false, false}; break;  // Default rising
    }
    return f;
  }

  static inline std::size_t pack_samples_to_words(
      const uint16_t *in_samples,
      std::size_t sample_count,
      uint16_t *out_words,
      std::size_t out_capacity)
  {
    if (sample_count == 0) return 0;
    std::size_t word_count = (sample_count + 3) / 4;
    if (word_count > out_capacity) word_count = out_capacity;

    std::size_t remainder = sample_count % 4;
    std::size_t read_index = 0;
    std::size_t write_index = 0;

    if (remainder != 0 && write_index < word_count)
    {
      uint16_t w = 0;
      for (std::size_t i = 0; i < remainder && read_index < sample_count; ++i)
      {
        uint16_t nibble = encode_sample_nibble(static_cast<uint16_t>(in_samples[read_index++] & 0x1));
        std::size_t nib_index = (4 - remainder) + i; // place remainder at high nibbles
        w |= static_cast<uint16_t>(nibble << (nib_index * 4));
      }
      w = harden_payload_word(w);
      out_words[write_index++] = w;
    }

    while (write_index < word_count && read_index < sample_count)
    {
      uint16_t w = 0;
      for (std::size_t i = 0; i < 4 && read_index < sample_count; ++i)
      {
        uint16_t nibble = encode_sample_nibble(static_cast<uint16_t>(in_samples[read_index++] & 0x1));
        std::size_t nib_index = 3 - i; // MSB-first in each word
        w |= static_cast<uint16_t>(nibble << (nib_index * 4));
      }
      w = harden_payload_word(w);
      out_words[write_index++] = w;
    }

    return write_index;
  }

private:
  static inline uint16_t harden_payload_word(uint16_t w)
  {
    uint8_t nib3 = static_cast<uint8_t>((w >> 12) & 0x0F);
    if (nib3 == 0x1)
    {
      uint8_t nib2 = static_cast<uint8_t>((w >> 8) & 0x0F);
      uint8_t nib1 = static_cast<uint8_t>((w >> 4) & 0x0F);
      uint8_t nib0 = static_cast<uint8_t>(w & 0x0F);
      uint8_t checksum = static_cast<uint8_t>((nib3 ^ nib2 ^ nib1) & 0x0F);
      if (nib0 == checksum)
      {
        nib0 ^= 0x2; // Flip bit 1, preserve sample LSB
        w = static_cast<uint16_t>(((nib3 & 0x0F) << 12) |
                                  ((nib2 & 0x0F) << 8)  |
                                  ((nib1 & 0x0F) << 4)  |
                                  (nib0 & 0x0F));
      }
    }
    return w;
  }

  static inline uint16_t encode_sample_nibble(uint16_t sample)
  {
    return (sample & 0x1) ? 0x5 : 0x0;
  }
};

#endif

// EOF
