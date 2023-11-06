#include "SystemVitals.hpp"

#include "System.hpp"

SystemVitals::SystemVitals(WifiController& wifiController, MqttClient& mqttClient)
: mWifiController(wifiController)
, mMqttClient(mqttClient)
{
    mSendTimer.sheduleIn(std::chrono::seconds(10), [this](auto& timer){
        send();
    });
}

void SystemVitals::send()
{
    char topic[256];
    char message[256];
    snprintf(topic, sizeof(topic) ,"bittailor/home/device/%s/vitals", System::id());
    snprintf(message, sizeof(message),
        R"JSON({"heap":{"free":%u,"total":%u},"rssi":%d,"uptime":%llu})JSON",
            System::freeHeap(),
            System::totalHeap(),
            mWifiController.rssi(),
            get_absolute_time()
    );
    mMqttClient.publish(topic, message);
    mSendTimer.sheduleIn(std::chrono::seconds(10), [this](auto& timer){
        send();
    });    
}
