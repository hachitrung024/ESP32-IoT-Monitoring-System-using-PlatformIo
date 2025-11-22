#include "relay.h"

void relay_task(void *pvParameters) {
  RelayCommand_t cmd;

  for (int id = 0; id < RELAY_COUNT; id++) {
    pinMode(relayPin[id], OUTPUT);
    digitalWrite(relayPin[id], LOW); // Initialize all relays to OFF
  }
  for (;;) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
    // Block and wait indefinitely for a command
    if (xQueueReceive(xRelayControlQueue, &cmd, portMAX_DELAY) == pdPASS) {
      // Check which relay to control
      if (cmd.target_id < RELAY_COUNT) {
        digitalWrite(relayPin[cmd.target_id], cmd.state ? HIGH : LOW);
        Serial.printf("Relay %d turned %s\n", cmd.target_id, cmd.state ? "ON" : "OFF");
      } else {
        Serial.printf("Invalid relay ID: %d\n", cmd.target_id);
      }
    }
  }
}