#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern float glob_temperature;  
extern float glob_humidity;     

extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern String AP_SSID;
extern String AP_PASSWORD;
extern String WIFI_SSID;
extern String WIFI_PASSWORD;

extern bool is_ap_mode;         
extern bool is_connecting;      
extern bool is_wifi_connected;  

// Semaphore Wait for Wifi connect
extern SemaphoreHandle_t xBinarySemaphoreInternet;
// Mutex protect Global Sensor data
extern SemaphoreHandle_t xGlobMutex;
// Mutex protect is_ap_mode
extern SemaphoreHandle_t xApModeMutex;
// Mutex protect is_connecting
extern SemaphoreHandle_t xConnectingMutex;
// Mutex protect is_wifi_connected
extern SemaphoreHandle_t xWifiConnectedMutex;

// --- Relay Definitions ---
/* How to control the relays:
   Send a RelayCommand_t structure to the xRelayControlQueue queue.
   The relay task will read from this queue and control the relays accordingly.
*/

// Relay Identifiers
typedef enum {
    RELAY_0,
    RELAY_1,
    // Add more relays here
    RELAY_COUNT
} RelayID_t;

// Relay GPIO mapping
static const int relayPin[RELAY_COUNT] = {
    [RELAY_0]   = GPIO_NUM_48,
    [RELAY_1]   = GPIO_NUM_10,
    // Add more relay pins here
};

// Relay Command Structure
typedef struct {
    uint8_t target_id;      // 0 for Relay0, 1 for Relay1
    bool    state;       // true (ON) or false (OFF)
} RelayCommand_t;

// Queue for Relay Control Commands
extern QueueHandle_t xRelayControlQueue;

// --- RS485 UART Definitions ---
extern HardwareSerial SERIAL_RS485; // RS485 to UART Module
#define SERIAL_RS485_BAUD 9600
#define SERIAL_RS485_CONFIG SERIAL_8N1
#define SERIAL_RS485_TX_PIN 7
#define SERIAL_RS485_RX_PIN 6

#define I2C_SDA 11
#define I2C_SCL 12
// --- Sensor Definitions ---
// Sensor Identifiers
typedef enum {
    DHT_SENSOR,
    NPK_SENSOR,
    // Add more sensors here
    SENSOR_COUNT
} SensorID_t;

// Event Group for Sensor Tasks
#define SENSOR_TRIGGER_READ_ALL_BIT (1 << 0)

extern EventGroupHandle_t xSensorEventGroup;

// Queue for Sensor Data
extern QueueHandle_t xSensorDataQueue;

extern SemaphoreHandle_t xSensorDataMutex;

// Function to get sensor data as a JSON string
String getSensorDataJsonString();

#endif