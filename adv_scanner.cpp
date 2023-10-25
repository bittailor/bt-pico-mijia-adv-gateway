#include "adv_scanner.hpp"

#include <stdio.h>

#include <pico/stdlib.h>

#include <btstack.h>
#include <btstack_event.h>


namespace {
    static constexpr uint16_t MIJIA_ADV_UUID = 0x181A;

    BleAdvScanner *sInstance = nullptr;
    std::function<void(const SensorData&)> sCallback;
    btstack_packet_callback_registration_t sHciEventCallbackRegistration;

    static void handleSensorData(bd_addr_t& address, uint8_t rssi, const uint8_t * data, uint8_t size) {
        if (size == 15) {
            float temperature = static_cast<int16_t>((data[7] << 8) | (data[6])) * 0.01;
            float humidity = ((data[9] << 8) | (data[8])) * 0.01;
            float batteryVoltage = ((data[11] << 8) | (data[10])) * 0.001;
            uint8_t batteryLevel = data[12];
            uint8_t cnt = data[13];
            if(sCallback) {
                SensorData data{
                    .temperature = temperature,
                    .humidity = humidity,
                    .batteryVoltage = batteryVoltage,
                    .batteryLevel = batteryLevel,
                    .mac = {address[0], address[1], address[2], address[3], address[4], address[5]},
                    .rssi = rssi
                };
                sCallback(data);
            }
        }
    }

    void handleAdvertisementData(bd_addr_t& address, uint8_t rssi, const uint8_t * adv_data, uint8_t adv_size){
        ad_context_t context;
        for (ad_iterator_init(&context, adv_size, (uint8_t *)adv_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
            uint8_t data_type    = ad_iterator_get_data_type(&context);
            uint8_t size         = ad_iterator_get_data_len(&context);
            const uint8_t * data = ad_iterator_get_data(&context);
            if(data_type == BLUETOOTH_DATA_TYPE_SERVICE_DATA) {
                uint16_t uuid = little_endian_read_16(data, 0);
                if(MIJIA_ADV_UUID == uuid) {
                    handleSensorData(address, rssi, data+2, size-2);
                }
            }          
        }
    }

    void packetHandler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
        UNUSED(size);
        UNUSED(channel);
        bd_addr_t local_addr;
        bd_addr_t address;
        uint8_t address_type;
        uint8_t event_type;
        int8_t rssi;
        uint8_t length;
        const uint8_t * data;

        if (packet_type != HCI_EVENT_PACKET) return;
        auto type = hci_event_packet_get_type(packet);
        switch(type){
            case BTSTACK_EVENT_STATE:
                if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
                gap_local_bd_addr(local_addr);
                printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
                break;
            case GAP_EVENT_ADVERTISING_REPORT:
                gap_event_advertising_report_get_address(packet, address);
                event_type = gap_event_advertising_report_get_advertising_event_type(packet);
                address_type = gap_event_advertising_report_get_address_type(packet);
                rssi = gap_event_advertising_report_get_rssi(packet);
                length = gap_event_advertising_report_get_data_length(packet);
                data = gap_event_advertising_report_get_data(packet);
                handleAdvertisementData(address, rssi, data, length);
                break;
            default:
                break;
        }
    }
}

BleAdvScanner::BleAdvScanner(std::function<void(const SensorData &)> callback)
{
    if(sInstance != nullptr) {
        printf("ERROR: BleAdvScanner already instantiated\n");
        return;
    }
    sInstance = this;
    sCallback = callback;
    sHciEventCallbackRegistration.callback = &packetHandler;
    hci_add_event_handler(&sHciEventCallbackRegistration); 
    gap_set_scan_parameters(1,48,48);
    gap_start_scan();    
    hci_power_control(HCI_POWER_ON); 
}
