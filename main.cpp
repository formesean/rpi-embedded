#include "MCP23S17.hpp"

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    try
    {
        MCP23S17 mcp(0, 21, 20, 19, 16);

        mcp.direction(MCP23S17::PORT::A, MCP23S17::DIRECTION::OUTPUT);

        while (true)
        {
            mcp.write(MCP23S17::PORT::A, 0x01);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            mcp.write(MCP23S17::PORT::A, 0x00);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
