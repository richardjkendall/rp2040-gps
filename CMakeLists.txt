# What CMake to start at
cmake_minimum_required(VERSION 3.12)
 
# Include the subsidiary .cmake file to get the SDK
include(pico_sdk_import.cmake)
 
# Set the name and version of the project
project(rp2040-gps VERSION 1.0.0)
 
# Link the Project to a source file (step 4.6)
add_executable(rp2040-gps source.c)
 
# Link the Project to an extra library (pico_stdlib)
target_link_libraries(rp2040-gps pico_stdlib)
 
# Initalise the SDK
pico_sdk_init()
 
# Enable USB, disable UART output
pico_enable_stdio_usb(rp2040-gps 1)
pico_enable_stdio_uart(rp2040-gps 0)
 
# Enable extra outputs (SWD?)
pico_add_extra_outputs(rp2040-gps)
