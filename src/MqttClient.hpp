#pragma once

#include <cstdio>
#include <atomic>

#include <pico/stdlib.h>
#include <lwip/dns.h>
#include <lwip/apps/mqtt.h>

class MqttClient {
    public:
        MqttClient();
        ~MqttClient();

        void connect();

        void publish(const char* topic, const char* data);

    private:
        void connect(const ip_addr_t& ipAddr);
        
    private:
        mqtt_client_t* mClient;
        mqtt_connect_client_info_t mClientInfo;
        ip_addr_t mServerAddr;
        std::atomic<bool> mConnected;
};