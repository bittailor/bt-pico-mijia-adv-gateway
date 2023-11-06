#pragma once

#include <cstdint>

#include "CallbackTimer.hpp"

class WifiController {
    public:
        WifiController();

        bool connect();

        int32_t rssi();

    private:
        void checkConnection();
        void startReconnect();
        void checkReconnect();

        CallbackTimer mCheckConnectionTimer;
};
