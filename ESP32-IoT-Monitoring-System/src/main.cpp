#include "global.h"

#include "neo_blinky.h"

#include "tiny_ml.h"
#include "core_iot.h"
#include "relay.h"
#include "dht_sensor.h"
#include "npk_sensor.h"
#include "toogle_boot.h"
#include "check_info_file.h"
#include "sender_webserver.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin(I2C_SDA,I2C_SCL);

  checkInfoFile(0);

  xTaskCreate(neo_led_task, "NeoPixel Led Task", 2048, NULL, 2, NULL);
  xTaskCreate(toogle_boot_task, "Toggle Boot Task" ,4096  ,NULL  ,2 , NULL);
  // xTaskCreate(main_server_task, "Main Server Task" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
  xTaskCreate(relay_task, "Relay Control Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(dht_task, "DHT Sensor Task" ,4096  ,NULL  ,2 , NULL);
  // xTaskCreate(tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  // xTaskCreate(npk_sensor_task, "NPK Sensor Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(send_data_webserver_task, "Send Data Webserver Task" ,2048  ,NULL  ,2 , NULL);
}

void loop()
{
  if (checkInfoFile(1))
  {
    if (!reconnectWiFi())
    {
      stopWebserver();
    }
    else
    {
      //CORE_IOT_reconnect();
    }
  }
  else{
      reconnectWebserver();
  }
}