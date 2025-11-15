#ifndef __CORE_IOT_H__
#define __CORE_IOT_H__

#define CONFIG_THINGSBOARD_ENABLE_DEBUG false
#include "global.h"
#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <OTA_Firmware_Update.h>
#include <ThingsBoard.h>
#include <Server_Side_RPC.h>
#include <Shared_Attribute_Update.h>
#include <Attribute_Request.h>
#include <Espressif_Updater.h>
// Firmware title and version used to compare with remote version, to check if an update is needed.
// Title needs to be the same and version needs to be different --> downgrading is possible
constexpr char CURRENT_FIRMWARE_TITLE[] = "ESP32";
constexpr char CURRENT_FIRMWARE_VERSION[] = "0.0.0";
// Maximum amount of retries we attempt to download each firmware chunck over MQTT
constexpr uint8_t FIRMWARE_FAILURE_RETRIES = 12U;
// Size of each firmware chunck downloaded over MQTT,
// increased packet size, might increase download speed
constexpr uint16_t FIRMWARE_PACKET_SIZE = 32768U;
//RPC configuration
constexpr uint8_t MAX_RPC_SUBSCRIPTIONS = 1U;
constexpr uint8_t MAX_RPC_RESPONSE = 5U;
constexpr char CONNECTING_MSG[] = "Connecting to: (%s) with token (%s)";
constexpr char RPC_METHOD[] = "rpc_method";

// constexpr char WIFI_SSID[] = "hyperion2";
// constexpr char WIFI_PASSWORD[] = "qwerqwer";

constexpr char TOKEN[] = "M3ycLNYqVOVjcAvlEyx3";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint16_t MAX_MESSAGE_SEND_SIZE = 512U;
constexpr uint16_t MAX_MESSAGE_RECEIVE_SIZE = 512U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;
constexpr uint64_t REQUEST_TIMEOUT_MICROSECONDS = 10000U * 1000U;

void coreiot_task(void *pvParameters);

#endif

