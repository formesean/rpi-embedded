cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH "D:/pico-sdk")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_to_zero C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

include_directories(include)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
