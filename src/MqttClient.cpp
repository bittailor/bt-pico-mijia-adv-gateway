#include "MqttClient.hpp"

#include <cstring>

#include "System.hpp"

MqttClient::MqttClient() 
: mClient{nullptr}
, mClientInfo{}
, mServerAddr{}
, mReconnectTimer{} {
    mClient = mqtt_client_new();
    if (mClient == nullptr) {
        printf("Failed to allocate MQTT client.\n");
        return;
    }
    snprintf(mClientId, MAX_CLIENT_ID_LENGTH, "bt-pico-%s", System::id());
    mClientInfo.client_id = mClientId;
    #ifdef MQTT_USER
    mClientInfo.client_user = MQTT_USER;
    #endif
    #ifdef MQTT_PASSWORD
    mClientInfo.client_pass = MQTT_PASSWORD;
    #endif

    mClientInfo.keep_alive = 60;
}

void MqttClient::connect()
{
    printf("dns_gethostbyname ... \n");
    err_t dnsResult = dns_gethostbyname(MQTT_SERVER, &mServerAddr, [](const char *name, const ip_addr_t *ipaddr, void *callbackArg){
        MqttClient* self = static_cast<MqttClient*>(callbackArg);
        if (ipaddr != nullptr) {
            printf("MQTT dns callback '%s' resolved to '%s'\n", name, ipaddr_ntoa(ipaddr));
            self->connect(*ipaddr);
        } else {
            printf("MQTT dns callback '%s' not resolved => retry in 5s ... \n", name);
            self->mReconnectTimer.sheduleIn(std::chrono::seconds(5), [self](auto& timer){
                printf("MQTT ... 5s passed => reconnect\n");
                self->connect();
            });
        }
    }, this);
    printf("dns_gethostbyname => %d\n", dnsResult);
}

MqttClient::~MqttClient() {
    mqtt_client_free(mClient);
}

void MqttClient::publish(const char* topic, const char* data) {
    if(mqtt_client_is_connected(mClient) == 1) {
        printf("MQTT publish %s => %s\n", topic, data);
        auto err = mqtt_publish(mClient, topic, data, strlen(data), 0, 0, [](void *callbackArg, err_t err){
            MqttClient* self = static_cast<MqttClient*>(callbackArg);
            switch (err)
            {
                case ERR_OK: { 
                    printf("MQTT publish success\n");    
                } break;
                case ERR_TIMEOUT: { 
                    printf("MQTT publish timed out\n");    
                } break;
                default: { 
                    printf("MQTT publish failed %d\n", err);    
                } break;
            }     
        }, this);
        if(err != ERR_OK) {
            printf("MQTT publish error %d\n", err);
            if(err == ERR_CONN) {
                printf("MQTT publish error => not connected\n");        
            }        
        }
    } else {
        printf("MQTT publish ignored => not connected\n");    
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
                self->publish("bt/pico", "hello from gateway");
            }break;
            case MQTT_CONNECT_DISCONNECTED: {
                printf("MQTT disconnected => try to reconnect in 5s ...\n");
                self->mReconnectTimer.sheduleIn(std::chrono::seconds(5), [self](auto& timer){
                    printf("MQTT ... 5s passed => reconnect\n");
                    self->connect();
                });        
            }break;
            case MQTT_CONNECT_TIMEOUT: {
                printf("MQTT connect timeout => try to reconnect\n");
                self->connect();        
            }break;
            case MQTT_CONNECT_REFUSED_PROTOCOL_VERSION:
            case MQTT_CONNECT_REFUSED_IDENTIFIER:
            case MQTT_CONNECT_REFUSED_SERVER:
            case MQTT_CONNECT_REFUSED_USERNAME_PASS:
            case MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_: {
                printf("MQTT REFUSED (%d) => try to reconnec in 5min ...\n", status);
                self->mReconnectTimer.sheduleIn(std::chrono::minutes(5), [self](auto& timer){
                    printf("MQTT ... 5min passed => reconnect\n");
                    self->connect();
                });        
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
