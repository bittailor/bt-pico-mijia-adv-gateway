#ifndef BT_ADV_SCANNER_HPP
#define BT_ADV_SCANNER_HPP

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

class BleAdvScanner {
    public:
        BleAdvScanner(std::function<void(const SensorData&)> callback);
    private:
};

#endif