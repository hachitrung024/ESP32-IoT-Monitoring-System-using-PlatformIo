#include "rgb_led.h"

void setupRGB() {
  ledcSetup(RED_CHANNEL, 5000, 8); 
  ledcSetup(GREEN_CHANNEL, 5000, 8);
  ledcSetup(BLUE_CHANNEL, 5000, 8);

  ledcAttachPin(RGB_LED_RED_PIN, RED_CHANNEL);
  ledcAttachPin(RGB_LED_GREEN_PIN, GREEN_CHANNEL);
  ledcAttachPin(RGB_LED_BLUE_PIN, BLUE_CHANNEL);
}

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(RED_CHANNEL, r);
  ledcWrite(GREEN_CHANNEL, g);
  ledcWrite(BLUE_CHANNEL, b);
}

void rgb_led_task(void *pvParameters){
  setupRGB();
  
  while(1) {     
    bool ap_mode, connecting, wifi_connected;
    if (xSemaphoreTake(xApModeMutex, portMAX_DELAY) == pdTRUE) {
      ap_mode = is_ap_mode;
      xSemaphoreGive(xApModeMutex);
    }

    if (xSemaphoreTake(xConnectingMutex, portMAX_DELAY) == pdTRUE) {
      connecting = is_connecting;
      xSemaphoreGive(xConnectingMutex);
    }

    if (xSemaphoreTake(xWifiConnectedMutex, portMAX_DELAY) == pdTRUE) {
      wifi_connected = is_wifi_connected;
      xSemaphoreGive(xWifiConnectedMutex);
    }

    if (ap_mode)
    {
        setRGB(0,0,255);
        vTaskDelay(1000);
    }
    else if (connecting)
    {
        setRGB(255,0,0);
        vTaskDelay(100);
    }
    else if (wifi_connected)
    {
        setRGB(0,255,0);
    }
    else{
        setRGB(0,0,0);
    }
  }
}