cmake_minimum_required(VERSION 3.12)

# Set the SDK path explicitly
set(PICO_SDK_PATH "D:/pico-sdk")

# Include build functions from the PICO SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Initialize the SDK
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cpp
    ../../lib/SPICO/SPICO.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE lib/SPICO)

# Link to standard Pico libraries
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi)

# Create map/bin/hex files
pico_add_extra_outputs(${PROJECT_NAME})
