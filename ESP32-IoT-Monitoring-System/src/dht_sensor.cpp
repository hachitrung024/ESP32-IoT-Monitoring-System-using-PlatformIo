#include "dht_sensor.h"

void dht_task (void * pvParameter){
    float temperature = 0.0f, humidity = 0.0f;
    char json[64];
    DHT dht(DHT_PIN, DHT_TYPE);
    dht.begin();
    for(;;){
        EventBits_t uxBits = xEventGroupWaitBits(
            xSensorEventGroup,
            SENSOR_TRIGGER_READ_ALL_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println(F("[DHT] Failed to read from DHT sensor!"));
            temperature = -1.0f;
            humidity = -1.0f;
        }
        // Serial.printf("[DHT%d] Temp: %.2f°C | Humi:%.2f%%\n", DHT_TYPE, temperature, humidity);
        snprintf(json, sizeof(json), "{\"DHT-Temp\":%.2f,\"DHT-Humi\":%.2f}", temperature, humidity);
        if (xQueueSend(xSensorDataQueue, &json, 0) != pdPASS) {
            Serial.println("[DHT] Failed to send telemetry data to queue");
        }
    }
    
}
