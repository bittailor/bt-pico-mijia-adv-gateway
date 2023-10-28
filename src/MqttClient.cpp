#include "MqttClient.hpp"

#include <cstring>

#include "System.hpp"

MqttClient::MqttClient() : mClientInfo{} {
    mClient = mqtt_client_new();
    if (mClient == nullptr) {
        printf("Failed to allocate MQTT client.\n");
        return;
    }
    mClientInfo.client_id = systemId();
    mClientInfo.client_user = MQTT_USER;
    mClientInfo.client_pass = MQTT_PASSWORD;
    mClientInfo.keep_alive = 60;
}

void MqttClient::connect()
{
    printf("dns_gethostbyname ... \n");
    err_t dnsResult = dns_gethostbyname(MQTT_SERVER, &mServerAddr, [](const char *name, const ip_addr_t *ipaddr, void *callback_arg){
        MqttClient* self = static_cast<MqttClient*>(callback_arg);
        if (ipaddr != nullptr) {
            printf("DNS callback resolved %s =>  %s\n", name, ipaddr_ntoa(ipaddr));
            self->connect(*ipaddr);
        } else {
            printf("DNS callback not resolved %s => ? \n", name);
        }
    }, this);
    printf("dns_gethostbyname => %d\n", dnsResult);
}

MqttClient::~MqttClient() {
    mqtt_client_free(mClient);
}

void MqttClient::publish(const char* topic, const char* data) {
    if(mConnected) {
        auto err = mqtt_publish(mClient, topic, data, strlen(data), 0, 0, [](void *arg, err_t err){
            printf("MQTT publish status %d\n", err);        
        }, this);
        if(err != ERR_OK) {
            printf("MQTT publish error %d\n", err);        
        }
    }   
}
         
void MqttClient::connect(const ip_addr_t& ipAddr) {
    printf("Connecting to MQTT broker...\n");
    int mqttConnectResult = mqtt_client_connect(mClient, &ipAddr, 1883, [](mqtt_client_t *client, void *arg, mqtt_connection_status_t status){
        MqttClient* self = static_cast<MqttClient*>(arg);
        printf("MQTT connection status %d\n", status); 
        switch (status)
        {
            case MQTT_CONNECT_ACCEPTED: {
                self->mConnected = true;    
                self->publish("bt/pico", "hello from gateway");
            }break;
            case MQTT_CONNECT_DISCONNECTED: {
                self->mConnected = false;
                printf("MQTT disconnected => try to reconnect\n");
                self->connect();
            }break;   
        }
    }, this, &mClientInfo);
    if (mqttConnectResult == 0) {
        printf("Connected.\n");
    } else {
        printf("failed to connect error code = %d => retry to connect\n", mqttConnectResult);
        connect();
    }
}
