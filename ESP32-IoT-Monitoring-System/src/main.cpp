#include "global.h"

#include "rgb_led.h"

#include "main_server.h"
#include "tiny_ml.h"
#include "core_iot.h"

void setup()
{
  Serial.begin(115200);

  xTaskCreate(rgb_led_task, "RGB Led Task", 2048, NULL, 2, NULL);
  xTaskCreate(main_server_task, "Main Server Task" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
}

void loop()
{
  
}