#include "adv_scanner.hpp"

#include <stdio.h>

#include <btstack.h>
#include <btstack_event.h>

static constexpr uint16_t ADV_UUID = 0x181A;

static btstack_packet_callback_registration_t hci_event_callback_registration;


static void handle_sensor_data(bd_addr_t& address, const uint8_t * data, uint8_t size) {
    //printf("Service Data [%s] size = %d\n ", bd_addr_to_str(address), size);
    //printf_hexdump(data, size);

    if (size == 15) {
        float temperature = static_cast<int16_t>((data[7] << 8) | (data[6])) * 0.01;
        float humidity = ((data[9] << 8) | (data[8])) * 0.01;
        float vbattery = ((data[11] << 8) | (data[10])) * 0.001;
        uint8_t battery = data[12];
        uint8_t cnt = data[13];
        printf("[%s] temperature=%.2f humidity=%.2f battery=%d vbattery=%.2f  \n", bd_addr_to_str(address), temperature, humidity, battery, vbattery);
    }


}

static void handle_advertisement_data(bd_addr_t& address, const uint8_t * adv_data, uint8_t adv_size){
    ad_context_t context;
    uint8_t uuid_128[16];
    for (ad_iterator_init(&context, adv_size, (uint8_t *)adv_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type    = ad_iterator_get_data_type(&context);
        uint8_t size         = ad_iterator_get_data_len(&context);
        const uint8_t * data = ad_iterator_get_data(&context);
        
        switch (data_type) {
            case BLUETOOTH_DATA_TYPE_SERVICE_DATA: {
                    uint16_t uuid = little_endian_read_16(data, 0);
                    if(ADV_UUID == uuid) {
                        handle_sensor_data(address, data+2, size-2);
                        //printf("Service Data [%s] %x => ", bd_addr_to_str(address), uuid);
                        //printf_hexdump(data, size);
                        //printf("\n");
                    }
                }
                break;
            default:
                break;
        }          
    }
}


static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
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
            
            /*
            if(address[0] != 0xA4) {
                return;
            }
            */


            //printf("Advertisement (legacy) event: evt-type %u, addr-type %u, addr %s, rssi %d, data[%u] ", event_type,
            //   address_type, bd_addr_to_str(address), rssi, length);
            //printf_hexdump(data, length);
            handle_advertisement_data(address, data, length);
            break;
        default:
            break;
    }
}

void adv_scanner()
{
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration); 

    gap_set_scan_parameters(1,48,48);
    gap_start_scan();

    hci_power_control(HCI_POWER_ON); 

    btstack_run_loop_execute();   
}