#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String AP_SSID = "ESP32-AP";
String AP_PASSWORD = "12345678";
String WIFI_SSID = "A18.02";
String WIFI_PASSWORD = "18021802";

bool is_ap_mode = true;
bool is_connecting = false;
bool is_wifi_connected = false;

SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
SemaphoreHandle_t xGlobMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xApModeMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xConnectingMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xWifiConnectedMutex = xSemaphoreCreateMutex();
