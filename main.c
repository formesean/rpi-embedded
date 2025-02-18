#include <stdio.h>
#include <stdint.h>
#include <pigpio.h>

// SPI channel and speed configuration
#define SPI_CHANNEL 0     // Using SPI0 CE0 or CE1 (adjust if needed)
#define SPI_SPEED 1000000 // 1 MHz SPI clock

// MCP23S17 Registers
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCON 0x0A
#define GPIOA 0x12
#define GPIOB 0x13
#define OLATA 0x14
#define OLATB 0x15

// MCP23S17 base address (A0,A1,A2=0 => 0x40 write, 0x41 read)
#define MCP23S17_ADDR 0x40

static int spiHandle;

void writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t buffer[3] = {MCP23S17_ADDR, reg, value};
    int status = spiWrite(spiHandle, (char *)buffer, 3);

    if (status < 0)
    {
        printf("SPI Write Failed (status: %d)\n", status);
    }
    else
    {
        printf("SPI Write Successful (status: %d)\n", status);
    }
    fflush(stdout);
}

uint8_t readRegister(uint8_t reg)
{
    uint8_t txBuf[3] = {(uint8_t)(MCP23S17_ADDR | 0x01), reg, 0x00};
    uint8_t rxBuf[3] = {0};

    int status = spiXfer(spiHandle, (char *)txBuf, (char *)rxBuf, 3);
    if (status < 0)
    {
        printf("SPI Read Failed (status: %d)\n", status);
        return 0xFF;
    }
    else
    {
        printf("SPI Read Successful (status: %d)\n", status);
        return rxBuf[2]; // Received data is in the 3rd byte
    }
}

void writeGPIOA(uint8_t value)
{
    writeRegister(OLATA, value);
}

uint8_t readGPIOB(void)
{
    return readRegister(GPIOB);
}

void setupMCP23S17(void)
{
    // Set PORTA as outputs (0x00 => all pins output)
    writeRegister(IODIRA, 0x00);

    // Set PORTB as inputs (0xFF => all pins input)
    writeRegister(IODIRB, 0xFF);

    // IOCON to enable hardware address pins and keep sequential mode enabled
    writeRegister(IOCON, 0x08);

    writeGPIOA(0x00);
}

int main(void)
{
    if (gpioInitialise() < 0)
    {
        printf("pigpio initialization failed!\n");
        return -1;
    }

    // Open SPI channel
    spiHandle = spiOpen(SPI_CHANNEL, SPI_SPEED, 0); // mode 0 by default
    if (spiHandle < 0)
    {
        printf("Failed to open SPI!\n");
        gpioTerminate();
        return -1;
    }

    // Configure MCP23S17
    setupMCP23S17();

    // Simple blink loop on GPIOA pin0
    // Toggle OLATA bit0 on and off
    while (1)
    {
        // Turn LED on (assuming LED is connected to A0)
        writeGPIOA(0x01);
        gpioDelay(500000); // Delay 500 ms

        // Turn LED off
        writeGPIOA(0x00);
        gpioDelay(500000); // Delay 500 ms
    }

    spiClose(spiHandle);
    gpioTerminate();

    return 0;
}
