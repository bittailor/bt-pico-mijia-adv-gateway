#include "WifiController.hpp"

#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

WifiController::WifiController()
{
}

bool WifiController::connect()
{
    cyw43_arch_enable_sta_mode();
    printf("Connecting to WiFi ...\n");
    int connectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 50000);
    if (connectResult == 0) {
        printf("Connected.\n");
        return true;
    } else {
        printf("failed to connect error code = %d.\n", connectResult);
        return false;
    }
}
