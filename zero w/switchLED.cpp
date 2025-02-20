#include "../lib/MCP23S17.hpp"

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
        mcp.direction(MCP23S17::PORT::B, MCP23S17::DIRECTION::INPUT);

        while (true)
        {
            uint8_t switchState = mcp.read(MCP23S17::PORT::B);

            if (switchState == 0x01)
            {
                mcp.write(MCP23S17::PORT::A, 0x01);
            }
            else
            {
                mcp.write(MCP23S17::PORT::A, 0x00);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
