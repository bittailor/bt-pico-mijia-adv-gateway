#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <pico/cyw43_arch.h>

#include "BleAdvertisementScanner.hpp"
#include "WifiController.hpp"
#include "MqttClient.hpp"

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

    printf("\n\n\n********************************\n");
    printf("Startup\n");

    char boardId[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
    pico_get_unique_board_id_string(boardId, sizeof(boardId));
    printf("Board ID = %s\n", boardId);


    //if (cyw43_arch_init()) {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_SWITZERLAND)) {
        printf("WiFi init failed\n");
        return -1;
    }

    ledToggle(5);
    
    WifiController wifiController{};
    
    while(!wifiController.connect()) {
        printf("retry in 1s\n");
        sleep_ms(1000);
    }

    MqttClient mqttClient{};
    mqttClient.connect();
    
    BleAdvertisementScanner scanner{[&mqttClient](auto& data){
        printf("MAC: %s\n", bd_addr_to_str(data.mac));
        printf("  RSSI: %d\n", data.rssi);
        printf("  Temperature: %f\n", data.temperature);
        printf("  Humidity: %f\n", data.humidity);
        printf("  Battery Voltage: %f\n", data.batteryVoltage);
        printf("  Battery Level: %d\n", data.batteryLevel);    
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

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
    }

    return 0;
}
