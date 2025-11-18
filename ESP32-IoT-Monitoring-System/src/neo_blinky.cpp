#include "neo_blinky.h"

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

static void neoSetColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
}

void neo_led_task(void *pvParameters) {
  strip.begin();
  strip.clear();
  strip.show();

  bool ap_mode = false;
  bool connecting = false;
  bool wifi_connected = false;

  while (1) {
    if (xSemaphoreTake(xApModeMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
      ap_mode = is_ap_mode;
      xSemaphoreGive(xApModeMutex);
    }
    if (xSemaphoreTake(xConnectingMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
      connecting = is_connecting;
      xSemaphoreGive(xConnectingMutex);
    }
    if (xSemaphoreTake(xWifiConnectedMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
      wifi_connected = is_wifi_connected;
      xSemaphoreGive(xWifiConnectedMutex);
    }

    if (ap_mode) {
      // AP mode: Blue, hold 1000 ms
      neoSetColor(0, 0, 255);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    else if (connecting) {
      // Connecting: Red, hold 100 ms
      neoSetColor(255, 0, 0);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    else if (wifi_connected) {
      // Connected: Green
      neoSetColor(0, 255, 0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    else {
      neoSetColor(0, 0, 0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}