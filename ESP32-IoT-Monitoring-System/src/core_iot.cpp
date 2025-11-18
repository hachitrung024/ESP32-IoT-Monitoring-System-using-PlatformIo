#include "core_iot.h"
constexpr int16_t telemetrySendInterval = 5000U;
uint32_t previousDataSend = 0;

//Shared Attributes Configuration
constexpr uint8_t MAX_ATTRIBUTES = 1;
constexpr std::array<const char*, MAX_ATTRIBUTES> SHARED_ATTRIBUTES = {
  "POWER"
};
void requestTimedOut() {
  Serial.printf("Attribute request timed out did not receive a response in (%llu) microseconds. Ensure client is connected to the MQTT broker and that the keys actually exist on the target device\n", REQUEST_TIMEOUT_MICROSECONDS);
}
// Initialize underlying client, used to establish a connection
WiFiClient espClient;
// Initalize the Mqtt client instance
Arduino_MQTT_Client mqttClient(espClient);
// Initialize used apis
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, MAX_RPC_RESPONSE> rpc;
Shared_Attribute_Update<1U, MAX_ATTRIBUTES> shared_update;
Attribute_Request<2U, MAX_ATTRIBUTES> attr_request;
OTA_Firmware_Update<> ota;
const std::array<IAPI_Implementation*, 4U> apis = {
    &rpc,
    &shared_update,
    &attr_request,
    &ota
};
// Initialize ThingsBoard instance with the maximum needed buffer size
ThingsBoard tb(mqttClient, MAX_MESSAGE_RECEIVE_SIZE, MAX_MESSAGE_SEND_SIZE, Default_Max_Stack_Size, apis);
// Initalize the Updater client instance used to flash binary to flash memory
Espressif_Updater<> updater;
// Statuses for updating
bool rpc_subscribed = false;
bool shared_update_subscribed = false;
bool currentFWSent = false;
bool updateRequestSent = false;
bool requestedShared = false;

static void updateStartingCallback() {
  Serial.println("Starting firmware update...");
}
void finishedCallback(const bool & success) {
  if (success) {
    Serial.println("Done, Reboot now");
    esp_restart();
    return;
  }
  Serial.println("Firmware update failed");
}
void progressCallback(const size_t & current, const size_t & total) {
  Serial.printf("Progress %.2f%%\n", static_cast<float>(current * 100U) / total);
}
static void vCheckFWUpdateTask(void * pvParameters){
  vTaskDelay(10000);
  const OTA_Update_Callback callback(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater, &finishedCallback, &progressCallback, &updateStartingCallback, FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE);
  Serial.print("");
  if(ota.Start_Firmware_Update(callback)) Serial.println("Done");
  vTaskDelete(NULL);
}
void handlePOWER1(const JsonVariantConst &data, JsonDocument &response){
  Serial.println("Received POWER1 RPC request");
  bool newState = false;  
  if (data.is<bool>()) {
      newState = data.as<bool>();
  } else {
      Serial.println("Error: RPC data was not a boolean.");
      return; // Ignore invalid data
  }
  RelayCommand_t cmd = {.target_id = 0, .state = newState};
  // Send the command to the relay task queue (non-blocking)
  if (xQueueSend(xRelayControlQueue, &cmd, 0) != pdPASS) {
      // Handle error if the queue is full
      Serial.println("Relay control queue is full!");
  }
}
void handlePOWER2(const JsonVariantConst &data, JsonDocument &response){
  bool newState = false;  
  if (data.is<bool>()) {
      newState = data.as<bool>();
  } else {
      Serial.println("Error: RPC data was not a boolean.");
      return; // Ignore invalid data
  }
  // Create the command structure
  RelayCommand_t cmd = {.target_id = 1, .state = newState};
  // Send the command to the relay task queue (non-blocking)
  if (xQueueSend(xRelayControlQueue, &cmd, 0) != pdPASS) {
      // Handle error if the queue is full
      Serial.println("Relay control queue is full!");
  }
}
void processSharedAttributeUpdate(const JsonObjectConst &data) {
  //Info
  const size_t jsonSize = Helper::Measure_Json(data);
  char buffer[jsonSize];
  serializeJson(data, buffer, jsonSize);
  Serial.println(buffer);
}

void processSharedAttributeRequest(const JsonObjectConst &data) {
  //Info
  const size_t jsonSize = Helper::Measure_Json(data);
  char buffer[jsonSize];
  serializeJson(data, buffer, jsonSize);
  Serial.println(buffer);
}

bool subscribeToAPIs(){
  if (!currentFWSent) {
    currentFWSent = ota.Firmware_Send_Info(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION);
  }
  if (!updateRequestSent) {
    Serial.print(CURRENT_FIRMWARE_TITLE);
    Serial.println(CURRENT_FIRMWARE_VERSION);
    const OTA_Update_Callback callback(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater, &finishedCallback, &progressCallback, &updateStartingCallback, FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE);
    Serial.print("Firwmare Update Subscription...");
    updateRequestSent = ota.Subscribe_Firmware_Update(callback);
    if(updateRequestSent) {
      Serial.println("Done");
      xTaskCreate(vCheckFWUpdateTask, "Check Firmware Update", 4096, NULL, 5, NULL);
    } else {
      Serial.println("Failed");
      return false;
    }
  }
  if (!rpc_subscribed){
    Serial.print("Subscribing for RPC...");
    const RPC_Callback callbacks[MAX_RPC_SUBSCRIPTIONS]= {
        {"POWER1", handlePOWER1},
        {"POWER2", handlePOWER2}
    };
    if(!rpc.RPC_Subscribe(callbacks + 0U, callbacks + MAX_RPC_SUBSCRIPTIONS)){
      Serial.println("Failed");
      return false;
    }
    Serial.println("Done");
    rpc_subscribed = true;
  }
  if (!shared_update_subscribed){
    Serial.print("Subscribing for shared attribute updates...");
    const Shared_Attribute_Callback<MAX_ATTRIBUTES> callback(&processSharedAttributeUpdate, SHARED_ATTRIBUTES);
    if (!shared_update.Shared_Attributes_Subscribe(callback)) {
    Serial.println("Failed");
    return false;
    }
    Serial.println("Done");
    shared_update_subscribed = true;
  }
  if (!requestedShared) {
    Serial.println("Requesting shared attributes...");
    const Attribute_Request_Callback<MAX_ATTRIBUTES> sharedCallback(&processSharedAttributeRequest, REQUEST_TIMEOUT_MICROSECONDS, &requestTimedOut, SHARED_ATTRIBUTES);
    requestedShared = attr_request.Shared_Attributes_Request(sharedCallback);
    if (!requestedShared) {
      Serial.println("Failed");
      return false;
    }
  }
  return true;
}

void coreiot_task(void * pvParameters){
  bool wifi_connected = false;
  bool apis_subscribed = false;
  String telemetry;

  for(;;){
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (xSemaphoreTake(xWifiConnectedMutex, portMAX_DELAY) == pdTRUE) {
      wifi_connected = is_wifi_connected;
      xSemaphoreGive(xWifiConnectedMutex);
    }
    if (!wifi_connected) {
      continue;
    }
    if (!tb.connected()) {
      // Reconnect to the ThingsBoard server,
      // if a connection was disrupted or has not yet been established
      Serial.printf("Connecting to: (%s) with token (%s)\n", THINGSBOARD_SERVER, TOKEN);
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
        Serial.println("Failed to connect, retrying in 5 seconds...");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        continue;
      }
      if(!apis_subscribed){
        apis_subscribed = subscribeToAPIs();
        continue;
      }
    }

    if (millis() - previousDataSend > telemetrySendInterval) {
      previousDataSend = millis();
      telemetry = getSensorDataJsonString();

      Serial.println("Sending telemetry: " + telemetry);
      tb.sendTelemetryString(telemetry.c_str());
    }

    tb.loop();
    vTaskDelay(10);
  }
}