#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>

#include "adv_scanner.hpp"


void blinky_loop() {
    uint32_t counter=0;
    while (true) {
        printf("core 1 %d\n", counter++);
        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
    }
}

int main()
{
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    
    printf("A\n");
    multicore_reset_core1();
    multicore_launch_core1(&blinky_loop); 
    printf("B\n");
    

    adv_scanner();

    return 0;
}
