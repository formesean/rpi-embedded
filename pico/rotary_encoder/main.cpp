#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>

#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <string.h>

#define SW_PIN 20
#define DT_PIN 19
#define CLK_PIN 18

void encoder_callback(uint gpio, uint32_t events)
{
  uint32_t gpio_state = 0;

  // Get the states of the GPIO pins (bits 18, 19, and 20)
  gpio_state = (gpio_get_all() >> 18) & 0b0111;

  static bool ccw_fall = 0;
  static bool cw_fall = 0;

  uint8_t enc_value = 0;
  enc_value = (gpio_state & 0x03); // Get the lower 2 bits (DT_PIN and CLK_PIN)

  if (gpio == DT_PIN)
  {
    if ((!cw_fall) && (enc_value == 0b10)) // Detect clockwise (CW)
      cw_fall = 1;

    if ((ccw_fall) && (enc_value == 0b00)) // Detect counterclockwise (CCW)
    {
      cw_fall = 0;
      ccw_fall = 0;
      printf("CCW \r\n");
    }
  }

  if (gpio == CLK_PIN)
  {
    if ((!ccw_fall) && (enc_value == 0b01)) // Detect counterclockwise (CCW)
      ccw_fall = 1;

    if ((cw_fall) && (enc_value == 0b00)) // Detect clockwise (CW)
    {
      cw_fall = 0;
      ccw_fall = 0;
      printf("CW \r\n");
    }
  }

  if (gpio == SW_PIN)
  {
    if (gpio_get(SW_PIN) == 0)
      printf("SW pressed!\r\n");
  }
}

int main()
{
  sleep_ms(500);
  stdio_init_all();

  // GPIO Setup for Encoder
  gpio_init(SW_PIN);
  gpio_set_dir(SW_PIN, GPIO_IN);
  gpio_pull_up(SW_PIN);

  gpio_init(DT_PIN);
  gpio_set_dir(DT_PIN, GPIO_IN);
  gpio_disable_pulls(DT_PIN);

  gpio_init(CLK_PIN);
  gpio_set_dir(CLK_PIN, GPIO_IN);
  gpio_disable_pulls(CLK_PIN);

  // Enable interrupts for SW, DT, and CLK pins with the proper callback function
  gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, encoder_callback);
  gpio_set_irq_enabled(DT_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
  gpio_set_irq_enabled(CLK_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

  while (1)
    sleep_ms(1000);

  return 0;
}
