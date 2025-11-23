#include "dht_sensor.h"
#if SOC_HP_I2C_NUM <= 1
TwoWire * I2C_Wire = &Wire;
#else
TwoWire * I2C_Wire = &Wire1;
#endif
void dht_task (void * pvParameter){
    float temperature = 0.0f, humidity = 0.0f;
    char json[64];
    DHT20 dht(&Wire);
    // DHT20 dht(I2C_Wire);
    Serial.println(__FILE__);
    Serial.print("DHT20 LIBRARY VERSION: ");
    Serial.println(DHT20_LIB_VERSION);
    Serial.println();
    delay(2000);
    // Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");

    bool ap_mode = true;
    for(;;){
    
        if (xSemaphoreTake(xApModeMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            ap_mode = is_ap_mode;
            xSemaphoreGive(xApModeMutex);
        }

        if (!ap_mode)
        {
            EventBits_t uxBits = xEventGroupWaitBits(
            xSensorEventGroup,
            SENSOR_TRIGGER_READ_ALL_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
            );
        }
    
        int status = dht.read();
        switch (status)
        {
            case DHT20_OK:
                // Serial.print("OK,\t");
                break;
                Serial.print("DHT20:");
            case DHT20_ERROR_CHECKSUM:
                Serial.print("Checksum error,\t");
                break;
            case DHT20_ERROR_CONNECT:
                Serial.print("Connect error,\t");
                break;
            case DHT20_MISSING_BYTES:
                Serial.print("Missing bytes,\t");
                break;
            case DHT20_ERROR_BYTES_ALL_ZERO:
                Serial.print("All bytes read zero");
                break;
            case DHT20_ERROR_READ_TIMEOUT:
                Serial.print("Read time out");
                break;
            case DHT20_ERROR_LASTREAD:
                Serial.print("Error read too fast");
                break;
            default:
                Serial.print("Unknown error,\t");
                break;
        }
        temperature = dht.getTemperature();
        humidity = dht.getHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println(F("Error: [DHT] Failed to read from DHT sensor!"));
            temperature = -1.0f;
            humidity = -1.0f;
        }
        
        if (xSemaphoreTake(xGlobMutex, portMAX_DELAY) == pdTRUE) {
            glob_temperature = temperature;
            glob_humidity = humidity;
            xSemaphoreGive(xGlobMutex);
        }
        if (!ap_mode)
        {
            snprintf(json, sizeof(json), "{\"DHT-Temp\":%.2f,\"DHT-Humi\":%.2f}", temperature, humidity);
            if (xQueueSend(xSensorDataQueue, json, 0) != pdPASS) {
                Serial.println("Error: [DHT] Failed to send telemetry data to queue");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        }
        else{
            Serial.printf("[DHT] Temp: %.2f°C | Humi:%.2f%%\n", temperature, humidity);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}
