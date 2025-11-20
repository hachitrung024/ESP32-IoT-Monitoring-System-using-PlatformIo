#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String AP_SSID = "ESP32-AP";
String AP_PASSWORD = "12345678";
String WIFI_SSID = "";
String WIFI_PASSWORD = "";

bool is_ap_mode = true;
bool is_connecting = false;
bool is_wifi_connected = false;

SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
SemaphoreHandle_t xGlobMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xApModeMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xConnectingMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xWifiConnectedMutex = xSemaphoreCreateMutex();

HardwareSerial SERIAL_RS485(1); // Use UART1 for RS485

// --- Queue Definition ---
// Define the actual Queue Handle variable
QueueHandle_t xRelayControlQueue = xQueueCreate(10, sizeof(RelayCommand_t));

SemaphoreHandle_t xSensorDataMutex = xSemaphoreCreateMutex();
EventGroupHandle_t xSensorEventGroup = xEventGroupCreate();
QueueHandle_t xSensorDataQueue = xQueueCreate(10, sizeof(char[64]));

String getSensorDataJsonString () {
    if(xSemaphoreTake(xSensorDataMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return "{}"; // Return empty JSON on failure to acquire mutex
    }
    xQueueReset(xSensorDataQueue);
    // Trigger all sensor tasks to read data
    xEventGroupSetBits(xSensorEventGroup, SENSOR_TRIGGER_READ_ALL_BIT);
    String json = "{";
    char buffer[64];
    bool first = true;
    for (int id = 0; id < SENSOR_COUNT; id++) {
        // Wait for each sensor task to signal completion, with timeout 2000 ms
        if (xQueueReceive(xSensorDataQueue, &buffer, pdMS_TO_TICKS(2000)) == pdPASS) {
            if (!first) {
                json += ",";
            }
            json += String(buffer).substring(1, String(buffer).length() - 1); // Remove the curly braces
            first = false;
        }
    }
    json += "}";
    xSemaphoreGive(xSensorDataMutex);
    return json;
}