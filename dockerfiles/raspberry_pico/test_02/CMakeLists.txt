cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK macros
# include(pico_sdk_import.cmake)
# include(/opt/pico-sdk/pico_sdk_init.cmake)
include(/opt/pico-sdk/external/pico_sdk_import.cmake)

project(picow_blink_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(blink_simple
  blink_simple.c
)

# Pull in the required library for managing the wireless-connected LED
# Link the correct CYW43 architecture library
# This automatically generates all required GCC flags and -I paths!
# -I /opt/pico-sdk/src/rp2_common/pico_cyw43_arch/include/
target_link_libraries(blink_simple
  pico_stdlib
  pico_cyw43_arch_none
)
# pico_cyw43_arch_none
# pico_cyw43_arch_lwip_threadsafe_background

# Enable usb/serial output if needed
pico_enable_stdio_usb(blink_simple 1)
pico_enable_stdio_uart(blink_simple 0)

# Create extra outputs including the UF2 file
pico_add_extra_outputs(blink_simple)

