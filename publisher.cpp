#include "publisher.hpp"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>


#include <lwip/dns.h>
#include <lwip/apps/mqtt.h>

class MqttClient {
    public:
        MqttClient() : mClientInfo{} {
            mClient = mqtt_client_new();
            if (mClient == nullptr) {
                printf("Failed to allocate MQTT client.\n");
                return;
            }
            mClientInfo.client_id = MQTT_CLIENT_ID;
            mClientInfo.client_user = MQTT_USER;
            mClientInfo.client_pass = MQTT_PASSWORD;
            mClientInfo.keep_alive = 60;


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

        ~MqttClient() {
            mqtt_client_free(mClient);
        }

    private:
        
        void connect(const ip_addr_t& ipAddr) {
            printf("Connecting to MQTT broker...\n");
            int mqttConnectResult = mqtt_client_connect(mClient, &ipAddr, 1883, [](mqtt_client_t *client, void *arg, mqtt_connection_status_t status){
                MqttClient* self = static_cast<MqttClient*>(arg);
                printf("MQTT connection status %d\n", status); 
                self->publish("bt/pico", "hello from gateway"); 

            }, this, &mClientInfo);
            if (mqttConnectResult == 0) {
                printf("Connected.\n");
            } else {
                printf("failed to connect error code = %d.\n", mqttConnectResult);
            }
        }

        
        void publish(const char* topic, const char* data) {
            mqtt_publish(mClient, topic, data, strlen(data), 0, 0, [](void *arg, err_t err){
                printf("MQTT publish status %d\n", err);        
            }, this);
        }
        
    private:
        mqtt_client_t* mClient;
        mqtt_connect_client_info_t mClientInfo;
        ip_addr_t mServerAddr;
};

static MqttClient* sMqttClient;

void connect() {
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    int connectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 50000);
    if (connectResult == 0) {
        printf("Connected.\n");
    } else {
        printf("failed to connect error code = %d.\n", connectResult);
    }

    sMqttClient = new MqttClient();

    /*
    sMqttClient = mqtt_client_new();
    if (sMqttClient == nullptr) {
        printf("Failed to allocate MQTT client.\n");
        return;
    }


     err_t err = dns_gethostbyname("", &(state->remote_addr), dns_found, state);

    mqtt_set_inpub_callback(sMqttClient, [](void *arg, const char *topic, const uint8_t *data, size_t len) {
        printf("Incoming publish at topic %s with data: ", topic);
        for (size_t i = 0; i < len; i++) {
            putchar((char) data[i]);
        }
        putchar('\n');
    }, nullptr);

    printf("Connecting to MQTT broker...\n");
    int mqttConnectResult = mqtt_client_connect(sMqttClient, MQTT_BROKER, MQTT_PORT, MQTT_KEEPALIVE);
    if (mqttConnectResult == 0) {
        printf("Connected.\n");
    } else {
        printf("failed to connect error code = %d.\n", mqttConnectResult);
    }
    */
}

void publishLoop() {
    uint32_t counter=0;
    while (true) {
        printf("core 1 %" PRIu32 "\n", counter++);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
    }

}