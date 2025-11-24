#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

uint32_t send_data_webserver_interval = 3000;
uint32_t telemetrySendInterval = 10000U;

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
SemaphoreHandle_t xBinarySemaphoreWebserver = xSemaphoreCreateBinary();
SemaphoreHandle_t xGlobMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xApModeMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xConnectingMutex = xSemaphoreCreateMutex();
SemaphoreHandle_t xWifiConnectedMutex = xSemaphoreCreateMutex();
// Semaphore to trigger NeoPixel update task
SemaphoreHandle_t xNeoUpdateSemaphore = xSemaphoreCreateBinary();

HardwareSerial SERIAL_RS485(1); // Use UART1 for RS485

// --- Queue Definition ---
// Define the actual Queue Handle variable
QueueHandle_t xRelayControlQueue = xQueueCreate(10, sizeof(RelayCommand_t));

SemaphoreHandle_t xSensorDataMutex = xSemaphoreCreateMutex();
EventGroupHandle_t xSensorEventGroup = xEventGroupCreate();
QueueHandle_t xSensorDataQueue = xQueueCreate(10, sizeof(char[64]));

char globalSensorJson[256]; 
String getSensorDataJsonString() {
    if (xSemaphoreTake(xSensorDataMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return "{}"; 
    }

    xQueueReset(xSensorDataQueue);
    xEventGroupSetBits(xSensorEventGroup, SENSOR_TRIGGER_READ_ALL_BIT);

    char newJson[256];
    strcpy(newJson, "{");

    char buffer[64];
    bool first = true;

    for (int i = 0; i < SENSOR_COUNT; i++) {

        if (xQueueReceive(xSensorDataQueue, buffer, pdMS_TO_TICKS(200)) == pdPASS) {

            String s = String(buffer);
            s = s.substring(1, s.length() - 1);

            if (!first) {
                strcat(newJson, ",");
            }

            strcat(newJson, s.c_str());
            first = false;
        }
    }

    strcat(newJson, "}");
    strcpy(globalSensorJson, newJson);

    xSemaphoreGive(xSensorDataMutex);
    // Serial.println("Sensor Data: " + String(globalSensorJson));

    return String(globalSensorJson);
}
