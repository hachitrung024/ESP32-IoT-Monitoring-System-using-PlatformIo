#include "global.h"

#include "rgb_led.h"

#include "main_server.h"
#include "tiny_ml.h"
#include "core_iot.h"

#include "relay.h"
#include "dht_sensor.h"
#include "npk_sensor.h"

void setup()
{
  Serial.begin(115200);

  xTaskCreate(rgb_led_task, "RGB Led Task", 2048, NULL, 2, NULL);
  xTaskCreate(main_server_task, "Main Server Task" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
  xTaskCreate(relay_task, "Relay Control Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(dht_task, "DHT Sensor Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(npk_sensor_task, "NPK Sensor Task" ,2048  ,NULL  ,2 , NULL);
}

void loop()
{
  // Sample code to print sensor data JSON every 5 seconds
  // Serial.println(getSensorDataJsonString());
  // vTaskDelay(pdMS_TO_TICKS(5000));
}