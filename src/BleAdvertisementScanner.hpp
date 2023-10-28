#pragma once

#include <cstdint>
#include <functional>

#include <btstack.h>

struct SensorData {
    float temperature;
    float humidity;
    float batteryVoltage;
    uint8_t batteryLevel;
    bd_addr_t mac;
    uint16_t rssi;
    uint8_t cnt; 
};

class BleAdvertisementScanner {
    public:
        BleAdvertisementScanner(std::function<void(const SensorData&)> callback);
    private:
};
