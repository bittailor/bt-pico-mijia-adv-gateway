add_executable(
  bt-pico-mijia-adv-gateway 
  main.cpp 
  System.cpp
  CallbackTimer.cpp
  BleAdvertisementScanner.cpp 
  WifiController.cpp
  MqttClient.cpp
  SystemVitals.cpp
)

target_compile_definitions(bt-pico-mijia-adv-gateway PRIVATE
  WIFI_SSID=\"${WIFI_SSID}\"
  WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
  MQTT_SERVER=\"${MQTT_SERVER}\"
  MQTT_CLIENT_ID=\"${MQTT_CLIENT_ID}\"
)

if(MQTT_USER)
  target_compile_definitions(bt-pico-mijia-adv-gateway PRIVATE
    MQTT_USER=\"${MQTT_USER}\"
  )  
endif() 
if(MQTT_PASSWORD)
  target_compile_definitions(bt-pico-mijia-adv-gateway PRIVATE
  MQTT_PASSWORD=\"${MQTT_PASSWORD}\"
  )  
endif() 

pico_set_program_name(bt-pico-mijia-adv-gateway "bt-pico-mijia-adv-gateway")
pico_set_program_version(bt-pico-mijia-adv-gateway "0.1")

pico_enable_stdio_uart(bt-pico-mijia-adv-gateway 1)
pico_enable_stdio_usb(bt-pico-mijia-adv-gateway 1)

# Add the standard library to the build
target_link_libraries(bt-pico-mijia-adv-gateway
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(bt-pico-mijia-adv-gateway PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(bt-pico-mijia-adv-gateway 
        pico_cyw43_arch_lwip_threadsafe_background
        pico_btstack_cyw43
        pico_btstack_ble
        #pico_multicore
        #pico_lwip_mbedtls
        #pico_mbedtls
        pico_lwip_mqtt
)

pico_add_extra_outputs(bt-pico-mijia-adv-gateway)

