#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <pico/cyw43_arch.h>
#include <pico/btstack_flash_bank.h>

#include "System.hpp"
#include "BleAdvertisementScanner.hpp"
#include "WifiController.hpp"
#include "MqttClient.hpp"
#include "SystemVitals.hpp"

void ledToggle(size_t n){
    for (size_t i = 0; i < n; i++)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(100);
    }
}

int main()
{
    stdio_init_all();
    printf("\n\n\n");
    puts("\033[2J");
    printf("============================================\n");
    printf("  BT MIJIA GATEWAY\n");
    printf("--------------------------------------------\n");
    printf("    System ID = %s\n", System::id());
    printf("    Code Size = %dk\n", System::codeSize()/1024);
    printf("    Heap\n");
    printf("      Total = %6.2f%k\n", System::totalHeap()/1024.0);
    printf("      Used  = %6.2fk\n", System::usedHeap()/1024.0);
    printf("      Free  = %6.2fk\n", System::freeHeap()/1024.0);
    printf("============================================\n");

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_SWITZERLAND)) {
        printf("WiFi init failed\n");
        return -1;
    }

    /*
    size_t address = XIP_BASE + PICO_FLASH_BANK_STORAGE_OFFSET;
    printf("address => %x \n", address);
    char buffer[100];
    char* test = (char*)(void*)(address);
    memcpy(buffer, test, 8);
    buffer[8]=0;
    printf("buffer => %s \n", buffer);
    */


    ledToggle(5);
    
    WifiController wifiController{};
    
    while(!wifiController.connect()) {
        printf("retry in 1s\n");
        sleep_ms(1000);
    }

    MqttClient mqttClient{};
    mqttClient.connect();
    
    BleAdvertisementScanner scanner{[&mqttClient](auto& data){
        /*
        printf("MAC: %s\n", bd_addr_to_str(data.mac));
        printf("  RSSI: %d\n", data.rssi);
        printf("  Temperature: %f\n", data.temperature);
        printf("  Humidity: %f\n", data.humidity);
        printf("  Battery Voltage: %f\n", data.batteryVoltage);
        printf("  Battery Level: %d\n", data.batteryLevel); 
        */   
        char topic[256];
        char message[256];
        snprintf(topic, sizeof(topic) ,"bittailor/home/sensor/xiaomi/%s/reading", bd_addr_to_str(data.mac));
        snprintf(message, sizeof(message) ,
            R"JSON({"rssi":%d,"temperature":%.2f,"humidity":%.2f,"battery":%.3f,"batteryLevel":%d,"cnt":%d})JSON",
                data.rssi,
                data.temperature,
                data.humidity,
                data.batteryVoltage,
                data.batteryLevel,
                data.cnt);
        mqttClient.publish(topic, message);
    }};

    
    SystemVitals systemVitals{wifiController, mqttClient};


    
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
    }

    return 0;
}
