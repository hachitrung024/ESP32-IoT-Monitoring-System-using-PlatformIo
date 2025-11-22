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
  // Signal-driven operation: wait until someone signals an update
  while (1) {
    // Wait indefinitely until signaled to update
    // xSemaphoreGive(xNeoUpdateSemaphore);
    if (xSemaphoreTake(xNeoUpdateSemaphore, portMAX_DELAY) == pdTRUE) {
      bool ap_mode = false;
      bool connecting = false;
      bool wifi_connected = false;

      // Read shared flags with short timeouts to avoid long blocking
      if (xSemaphoreTake(xApModeMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        ap_mode = is_ap_mode;
        xSemaphoreGive(xApModeMutex);
      }
      if (xSemaphoreTake(xConnectingMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        connecting = is_connecting;
        xSemaphoreGive(xConnectingMutex);
      }
      if (xSemaphoreTake(xWifiConnectedMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        wifi_connected = is_wifi_connected;
        xSemaphoreGive(xWifiConnectedMutex);
      }

      if (ap_mode) {
        neoSetColor(0, 0, 255); // AP: Blue
      } else if (connecting) {
        // Blink pattern while connecting
        neoSetColor(255, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        neoSetColor(0, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
      } else if (wifi_connected) {
        neoSetColor(0, 255, 0); // Connected: Green
      } else {
        neoSetColor(0, 0, 0); // Off
      }
    }
  }
}
