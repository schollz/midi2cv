# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(midi2cv C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(midi2cv main.c lib/midi.c)


target_link_libraries(midi2cv
        pico_stdlib 
        hardware_i2c
        hardware_uart
)

pico_set_program_name(midi2cv "midi2cv")
pico_set_program_version(midi2cv "0.1")

pico_enable_stdio_uart(midi2cv 0)
pico_enable_stdio_usb(midi2cv 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(midi2cv)

