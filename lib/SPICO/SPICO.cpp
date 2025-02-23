#include "SPICO.hpp"

// Constructor
SPICO::SPICO(spi_inst_t *spi_port, uint32_t baudrate, uint8_t cs_pin)
    : spi(spi_port), spi_baudrate(baudrate), cs_pin(cs_pin)
{
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, 1);
}

// SPI Initialization
void SPICO::begin(uint8_t miso, uint8_t mosi, uint8_t sclk)
{
  spi_init(spi, spi_baudrate);
  gpio_set_function(mosi, GPIO_FUNC_SPI);
  gpio_set_function(miso, GPIO_FUNC_SPI);
  gpio_set_function(sclk, GPIO_FUNC_SPI);
}

// SPI Transfer
uint8_t SPICO::transfer(uint8_t data)
{
  uint8_t rxData;
  spi_write_read_blocking(spi, &data, &rxData, 1);
  return rxData;
}

// Set SPI Clock Speed
void SPICO::setClockSpeed(uint32_t baudrate)
{
  spi_baudrate = baudrate;
  spi_set_baudrate(spi, baudrate);
}

// GPIO Functions
void SPICO::pinMode(uint8_t pin, DIRECTION dir)
{
  gpio_init(pin);
  gpio_set_dir(pin, dir == DIRECTION::OUTPUT ? GPIO_OUT : GPIO_IN);
}

void SPICO::digitalWrite(uint8_t pin, bool value)
{
  gpio_put(pin, value);
}

bool SPICO::digitalRead(uint8_t pin)
{
  return gpio_get(pin);
}

// MCP23S17 Write
void SPICO::mcp23s17_write(uint8_t reg, uint8_t data)
{
  uint8_t tx_buf[3] = {MCP23S17_ADDR, reg, data};

  gpio_put(cs_pin, 0);
  spi_write_blocking(spi, tx_buf, 3);
  gpio_put(cs_pin, 1);
}

// MCP23S17 Read
uint8_t SPICO::mcp23s17_read(uint8_t reg)
{
  uint8_t tx_buf[3] = {MCP23S17_ADDR | 0x01, reg, 0x00};
  uint8_t rx_buf[3] = {0};

  gpio_put(cs_pin, 0);
  spi_write_read_blocking(spi, tx_buf, rx_buf, 3);
  gpio_put(cs_pin, 1);

  return rx_buf[2];
}

void SPICO::mcp23s17_pinMode(PORT port, DIRECTION dir)
{
  uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::IODIRA)
                                  : static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::IODIRB);

  uint8_t directionMask = (dir == DIRECTION::INPUT) ? 0xFF : 0x00;

  mcp23s17_write(reg, directionMask);
}


void SPICO::mcp23s17_digitalWrite(PORT port, uint8_t data)
{
  uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::GPIOA)
                                  : static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::GPIOB);
  mcp23s17_write(reg, data);
}

uint8_t SPICO::mcp23s17_digitalRead(PORT port)
{
  uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::GPIOA)
                                  : static_cast<uint8_t>(MCP23S17_ADDR_BANK_0::GPIOB);
  return mcp23s17_read(reg);
}

void SPICO::delay(uint32_t ms)
{
  sleep_ms(ms);
}


// EOF
