#include "sender_webserver.h"

// Function to send sensor data as JSON via WebSocket
void sendSensorData() {
    float temperature = 0.0f;
    float humidity = 0.0f;

    if (xSemaphoreTake(xGlobMutex, portMAX_DELAY) == pdTRUE) {
        temperature = glob_temperature;
        humidity = glob_humidity;
        xSemaphoreGive(xGlobMutex);
    }

    String jsonData = "{";
    jsonData += "\"temperature\":" + String(temperature, 1) + ",";
    jsonData += "\"humidity\":" + String(humidity, 1);
    jsonData += "}";

    // Send to all WebSocket clients
    sendDataWebserver(jsonData);
}

void send_data_webserver_task(void *pvParameters)
{
    // Wait for webserver to be running
    xSemaphoreTake(xBinarySemaphoreWebserver, portMAX_DELAY);

    while (1)
    {
        Serial.printf("[SENDER]: Webserver is running: %d\n", webserver_isrunning);
        if (!webserver_isrunning)
        {
            Serial.println("Info: Webserver is not running, delaying send data task");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        
        Serial.println("Info: Sending sensor data to WebSocket clients");
        sendSensorData();
        vTaskDelay(send_data_webserver_interval / portTICK_PERIOD_MS);
    }
}