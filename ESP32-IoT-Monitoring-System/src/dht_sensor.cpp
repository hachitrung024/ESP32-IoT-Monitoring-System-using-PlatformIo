#include "dht_sensor.h"

void dht_task (void * pvParameter){
    float temperature = 0.0f, humidity = 0.0f;
    char json[64];
    DHT20 dht(&Wire);
    for(;;) {
        if(!dht.isConnected()){
            Serial.println("[DHT] sensor not connected!");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }
        // Wait for trigger to read sensor
        EventBits_t uxBits = xEventGroupWaitBits(
        xSensorEventGroup,
        SENSOR_TRIGGER_READ_ALL_BIT,
        pdTRUE,
        pdFALSE,
        portMAX_DELAY
        );
        auto start = millis();
        int status = dht.read();
        if(status != DHT20_OK){
            Serial.printf("Error: [DHT] Failed to read, error code: %d\n", status);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        temperature = dht.getTemperature();
        humidity = dht.getHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println(F("Error: [DHT] Failed to read from DHT sensor!"));
            temperature = 0.0f;
            humidity = 0.0f;
        }

        snprintf(json, sizeof(json), "{\"temperature\":%.2f,\"humidity\":%.2f}", temperature, humidity);
        if (xQueueSend(xSensorDataQueue, json, 0) != pdPASS) {
            Serial.println("Error: [DHT] Failed to send telemetry data to queue");
        }
        // Serial.printf("[DHT] Done (%dms)\n", millis() - start);

    }
}
