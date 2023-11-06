#pragma once

#include <cstdio>
#include <atomic>

#include <pico/stdlib.h>
#include <lwip/dns.h>
#include <lwip/apps/mqtt.h>

#include "CallbackTimer.hpp"

class MqttClient {
    public:
        MqttClient();
        ~MqttClient();

        void connect();

        void publish(const char* topic, const char* data);

    private:
        void connect(const ip_addr_t& ipAddr);
        
    private:
        static constexpr size_t MAX_CLIENT_ID_LENGTH = 24;
        mqtt_client_t* mClient;
        char mClientId[MAX_CLIENT_ID_LENGTH];
        mqtt_connect_client_info_t mClientInfo;
        ip_addr_t mServerAddr;
        CallbackTimer mReconnectTimer;
};