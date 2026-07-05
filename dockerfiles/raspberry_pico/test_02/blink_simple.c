#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Required library for Pico W/2W wireless chip

// /opt/pico-sdk/src/rp2_common/pico_cyw43_arch/include/pico/cyw43_arch.h

int main() {
    stdio_init_all();

    // Initialize the CYW43 architecture
    if (cyw43_arch_init()) {
        return -1;
    }

    while (true) {
        // Turn the LED on
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);

        // Turn the LED off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(100);
    }
}
