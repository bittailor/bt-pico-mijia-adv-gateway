#pragma once

#include "CallbackTimer.hpp"
#include "WifiController.hpp"
#include "MqttClient.hpp"

class SystemVitals {
    public:
        SystemVitals(WifiController& wifiController, MqttClient& mqttClient);
        
    private:
        void send();

        WifiController& mWifiController;
        MqttClient& mMqttClient;

        CallbackTimer mSendTimer;
};