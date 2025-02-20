#include "MCP23S17.hpp"

#include <iostream>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

int main()
{
    try
    {
        MCP23S17 mcp(CS_PIN, MISO_PIN, MOSI_PIN, SCLK_PIN, 250000);

        mcp.direction(MCP23S17::PORT::A, MCP23S17::DIRECTION::OUTPUT);

        while (true)
        {
            mcp.write(MCP23S17::PORT::A, 0x01);
            mcp.delay_ms(500);

            mcp.write(MCP23S17::PORT::A, 0x00);
            mcp.delay_ms(500);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
