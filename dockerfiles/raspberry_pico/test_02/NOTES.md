
The regular gpio_put(25, 1) C++ code fails on the Raspberry Pi Pico 2W because
the onboard LED is not connected to the main RP2350 microcontroller's GPIO
pins. Instead, the LED is routed through the wireless network chip (the CYW43
module).

docker run -it --privileged -v /dev/bus/usb:/dev/bus/usb intmain/microci_raspberry_pico:latest /opt/picotool/picotool info
