#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <cstdint>
#include <stdio.h>

// to build run these command:
//      mkdir build && cd build
//      cmake -G "MinGW Makefiles" ..
//      minGW32-make

#define LED_PIN 25

#define PIN_MISO 16 // RX
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_MOSI 19 // TX

#define MCP23S17_ADDR 0x40
#define IODIRA 0x00
#define GPIOA 0x12
#define OLATA 0x14

void mcp23s17_write(uint8_t reg, uint8_t data)
{
    uint8_t tx_buf[3] = {MCP23S17_ADDR, reg, data};

    gpio_put(PIN_CS, 0); // Select MCP23S17
    spi_write_blocking(spi0, tx_buf, 3);
    gpio_put(PIN_CS, 1); // Deselect MCP23S17
}

uint8_t mcp23s17_read(uint8_t reg)
{
    uint8_t tx_buf[3] = {MCP23S17_ADDR | 0x01, reg, 0x00};
    uint8_t rx_buf[3] = {0};

    gpio_put(PIN_CS, 0);
    spi_write_read_blocking(spi0, tx_buf, rx_buf, 3);
    gpio_put(PIN_CS, 1);

    return rx_buf[2]; // Return received byte
}

void setup_mcp23s17()
{
    // Ensure MCP23S17 is reset properly
    gpio_put(PIN_CS, 0);
    sleep_ms(10);
    gpio_put(PIN_CS, 1);
    sleep_ms(10);

    // Set all PORTA pins as outputs (IODIRA = 0x00)
    mcp23s17_write(IODIRA, 0x00);
}

void blink_led()
{
    while (true)
    {
        // Turn PORTA0 HIGH
        mcp23s17_write(GPIOA, 0x01);
        uint8_t readback = mcp23s17_read(GPIOA);
        printf("PORTA Status: 0x%02X\n", readback);

        gpio_put(LED_PIN, 1);
        sleep_ms(500);

        // Turn PORTA0 LOW
        mcp23s17_write(GPIOA, 0x00);
        readback = mcp23s17_read(GPIOA);
        printf("PORTA Status: 0x%02X\n", readback);

        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}

int main()
{
    stdio_init_all(); // Enable USB serial output
    spi_init(spi0, 250000);

    // Initialize GPIO pins for SPI communication
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configure Chip Select
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    setup_mcp23s17();

    blink_led(); // Blink both onboard LED and MCP23S17 LED

    return 0;
}
