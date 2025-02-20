#include "MCP23S17.hpp"

#include <iostream>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define ROW1 0b00000001
#define ROW2 0b00000010
#define COL1 0b00000001
#define COL2 0b00000010

void scanMatrix(MCP23S17 &mcp)
{
    mcp.direction(MCP23S17::PORT::A, MCP23S17::DIRECTION::INPUT);
    mcp.direction(MCP23S17::PORT::B, MCP23S17::DIRECTION::OUTPUT);
    mcp.enablePullup(MCP23S17::PORT::A, COL1 | COL2);

    while (true)
    {
        for (uint8_t row = 0; row < 2; row++)
        {
            // Drive only one row low at a time
            uint8_t rowMask = (row == 0) ? ROW1 : ROW2;
            mcp.write(MCP23S17::PORT::B, ~rowMask);

            uint8_t colState = mcp.read(MCP23S17::PORT::A);

            // Check which keys are pressed
            if (!(colState & COL1))
            {
                std::cout << "Key Pressed: Row " << row + 1 << " Col 1" << std::endl;
            }
            if (!(colState & COL2))
            {
                std::cout << "Key Pressed: Row " << row + 1 << " Col 2" << std::endl;
            }

            mcp.delay_ms(100);
        }
    }
}

int main()
{
    try
    {
        MCP23S17 mcp(CS_PIN, MISO_PIN, MOSI_PIN, SCLK_PIN, 250000);
        scanMatrix(mcp);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}

// EOF
