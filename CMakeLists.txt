# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "$ENV{HOME}/Code/PiPico/Cpp/pico-sdk")

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/secrets.cmake) 
  include(secrets.cmake)
else()
  include(secrets.defaults.cmake)   
endif()

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(bt-pico-mijia-adv-gateway C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(bt-pico-mijia-adv-gateway 
  main.cpp 
  adv_scanner.cpp 
  publisher.cpp
)

target_compile_definitions(bt-pico-mijia-adv-gateway PRIVATE
  WIFI_SSID=\"${WIFI_SSID}\"
  WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
  MQTT_SERVER=\"${MQTT_SERVER}\"
  MQTT_CLIENT_ID=\"${MQTT_CLIENT_ID}\"
  MQTT_USER=\"${MQTT_USER}\"
  MQTT_PASSWORD=\"${MQTT_PASSWORD}\"
)

pico_set_program_name(bt-pico-mijia-adv-gateway "bt-pico-mijia-adv-gateway")
pico_set_program_version(bt-pico-mijia-adv-gateway "0.1")

pico_enable_stdio_uart(bt-pico-mijia-adv-gateway 1)
pico_enable_stdio_usb(bt-pico-mijia-adv-gateway 0)

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
        pico_multicore
        #pico_lwip_mbedtls
        #pico_mbedtls
        pico_lwip_mqtt
)

pico_add_extra_outputs(bt-pico-mijia-adv-gateway)

