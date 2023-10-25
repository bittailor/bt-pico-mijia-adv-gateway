#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>

#include "adv_scanner.hpp"
#include "publisher.hpp"

int main()
{
    stdio_init_all();
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_SWITZERLAND)) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    connect();

    BleAdvScanner scanner{[](auto& data){
        printf("Temperature: %f\n", data.temperature);
        printf("Humidity: %f\n", data.humidity);
        printf("Battery Voltage: %f\n", data.batteryVoltage);
        printf("Battery Level: %d\n", data.batteryLevel);    
        printf("MAC: %s\n", bd_addr_to_str(data.mac));
        printf("RSSI: %d\n", data.rssi);
        printf("\n");
    }};


    publishLoop();
    
    /*
    printf("A\n");
    multicore_reset_core1();
    multicore_launch_core1(&mainCore1); 
    printf("B\n");
    mainCore0();
    */


    return 0;
}
