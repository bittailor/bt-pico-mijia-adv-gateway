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
        printf("Connected to %s RSSI=%d.\n", WIFI_SSID, rssi());
        checkConnection();
        return true;
    } else {
        printf("failed to connect error code = %d.\n", connectResult);
        return false;
    }
}

int32_t WifiController::rssi()
{
    int32_t rssi = 0;
    auto res = cyw43_wifi_get_rssi(&cyw43_state, &rssi);
    if (res != 0) {
        printf("failed to get RSSI = %d\n", res);
    }
    return rssi;
}

void WifiController::checkConnection()
{
    auto linkStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    if(linkStatus == CYW43_LINK_UP) {
        mCheckConnectionTimer.sheduleIn(std::chrono::seconds(1),[this](auto& timer){checkConnection();});
        return;
    }
    printf("WiFi link status = %d => Connection lost => try reconnct ...\n", linkStatus);
    mCheckConnectionTimer.sheduleIn(std::chrono::milliseconds(100),[this](auto& timer){checkReconnect();});
}

void WifiController::startReconnect()
{
    printf("WiFi Start reconnct\n");
    cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    mCheckConnectionTimer.sheduleIn(std::chrono::seconds(10),[this](auto& timer){checkReconnect();});
}

void WifiController::checkReconnect()
{
    auto linkStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    /*
     * link status        | Meaning
     * -------------------|--------
     * CYW43_LINK_DOWN    | Wifi down
     * CYW43_LINK_JOIN    | Connected to wifi
     * CYW43_LINK_NOIP    | Connected to wifi, but no IP address
     * CYW43_LINK_UP      | Connect to wifi with an IP address
     * CYW43_LINK_FAIL    | Connection failed
     * CYW43_LINK_NONET   | No matching SSID found (could be out of range, or down)
     * CYW43_LINK_BADAUTH | Authenticatation failure
     */
    switch (linkStatus)
    {
        case CYW43_LINK_UP: {
            printf("WiFi CYW43_LINK_UP => reconnected\n");
            mCheckConnectionTimer.sheduleIn(std::chrono::seconds(1),[this](auto& timer){checkConnection();});
        }break;

        case CYW43_LINK_JOIN: {
            printf("WiFi CYW43_LINK_JOIN => ... \n");
            mCheckConnectionTimer.sheduleIn(std::chrono::milliseconds(200),[this](auto& timer){startReconnect();});
        }break;
        case CYW43_LINK_NOIP: {
            printf("WiFi CYW43_LINK_NOIP => ... \n");
            mCheckConnectionTimer.sheduleIn(std::chrono::milliseconds(200),[this](auto& timer){startReconnect();});
        }break;

        case CYW43_LINK_FAIL: {
            printf("WiFi CYW43_LINK_FAIL => try reconnect in 10s \n");
            mCheckConnectionTimer.sheduleIn(std::chrono::seconds(10),[this](auto& timer){startReconnect();});
        }break;
        case CYW43_LINK_NONET: {
            printf("WiFi CYW43_LINK_NONET => try reconnect in 10s \n");
            mCheckConnectionTimer.sheduleIn(std::chrono::seconds(10),[this](auto& timer){startReconnect();});
        }break;
        case CYW43_LINK_BADAUTH: {
            printf("WiFi CYW43_LINK_BADAUTH => try reconnect in 10s \n");
            mCheckConnectionTimer.sheduleIn(std::chrono::seconds(10),[this](auto& timer){startReconnect();});
        }break;
        case CYW43_LINK_DOWN: {
            printf("WiFi CYW43_LINK_DOWN => try reconnect in 10s \n");
            cyw43_arch_enable_sta_mode();
            mCheckConnectionTimer.sheduleIn(std::chrono::seconds(10),[this](auto& timer){startReconnect();});
        }break;
    
        default:{
            printf("WiFi ? %d => recheck \n", linkStatus);
            mCheckConnectionTimer.sheduleIn(std::chrono::milliseconds(100),[this](auto& timer){checkReconnect();});
        }break;
    }    
}
