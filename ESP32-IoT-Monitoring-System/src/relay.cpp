#include "relay.h"
void relay_task(void *pvParameters) {
  RelayCommand_t cmd;

  // Initialize both pins
  pinMode(POWER1_PIN, OUTPUT);
  pinMode(POWER2_PIN, OUTPUT);
  xRelayControlQueue = xQueueCreate(10, sizeof(RelayCommand_t));
  for (;;) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
    // Block and wait indefinitely for a command
    if (xQueueReceive(xRelayControlQueue, &cmd, portMAX_DELAY) == pdPASS) {
      // Check which relay to control
      switch (cmd.target_id) {
        case 1:
            digitalWrite(POWER1_PIN, cmd.state ? HIGH : LOW);
          Serial.printf("[Relay] POWER1: %s\n", cmd.state ? "ON" : "OFF");
          break;
      
        case 2:
          digitalWrite(POWER2_PIN, cmd.state ? HIGH : LOW);
          Serial.printf("[Relay] POWER2: %s\n", cmd.state ? "ON" : "OFF");
          break;
      
        default:
          Serial.println("[Relay] Received command for unknown target.");
          break;
      }
    }
  }
}