#include "core_iot.h"

constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";

volatile bool attributesChanged = false;
volatile int ledMode = 0;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

uint32_t previousStateChange;

constexpr int16_t telemetrySendInterval = 10000U;
uint32_t previousDataSend;

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
  LED_STATE_ATTR,
  BLINKING_INTERVAL_ATTR
};

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

DHT20 dht20;

RPC_Response setLedSwitchState(const RPC_Data &data) {
    Serial.println("Received Switch state");
    bool newState = data;
    Serial.print("Switch state change: ");
    Serial.println(newState);
    digitalWrite(LED_PIN, newState);
    attributesChanged = true;
    return RPC_Response("setLedSwitchValue", newState);
}

const std::array<RPC_Callback, 1U> callbacks = {
  RPC_Callback{ "setLedSwitchValue", setLedSwitchState }
};

void processSharedAttributes(const Shared_Attribute_Data &data) {
  for (auto it = data.begin(); it != data.end(); ++it) {
    if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0) {
      const uint16_t new_interval = it->value().as<uint16_t>();
      if (new_interval >= BLINKING_INTERVAL_MS_MIN && new_interval <= BLINKING_INTERVAL_MS_MAX) {
        blinkingInterval = new_interval;
        Serial.print("Blinking interval is set to: ");
        Serial.println(new_interval);
      }
    } else if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0) {
      ledState = it->value().as<bool>();
      digitalWrite(LED_PIN, ledState);
      Serial.print("LED state is set to: ");
      Serial.println(ledState);
    }
  }
  // attributesChanged = true;
}

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());

void initWiFi() {
  // Need semaphore here
  if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY) == pdTRUE) {
    Serial.println("Connected to Wifi");
  }
}

const bool reconnectWifi() {
  // Check to ensure we aren't connected yet
  const wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    return true;
  }
  else{
    // If we aren't reconect Wifi
    Serial.println("Reconnecting to Wifi ...");
    
    unsigned long connect_start_ms = 0;
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      // Wait until a connection has been successfully
      delay(500);
      Serial.print(".");

      if (millis() - connect_start_ms > 10000)
      {
        // Timeout 10s
        Serial.println("WiFi connect failed! Press BOOT to back AP");

        if (xSemaphoreTake(xWifiConnectedMutex, portMAX_DELAY) == pdTRUE) {
          is_wifi_connected = false;
          xSemaphoreGive(xWifiConnectedMutex);
        }
        return false;
      }
    }

    return true;
  }
}

void setupCoreIot() {
  pinMode(LED_PIN, OUTPUT);
  vTaskDelay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();

  initWiFi();  
}

void coreiot_task(void *pvParameters){
    setupCoreIot();

    while(1){
      if (!reconnectWifi()) {
        vTaskDelay(3000);
        continue;
      }

      if (!tb.connected()) {
        Serial.print("Connecting to: ");
        Serial.print(THINGSBOARD_SERVER);
        Serial.print(" with token ");
        Serial.println(TOKEN);
        if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
          Serial.println("Failed to connect");
          return;
        }

        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

        Serial.println("Subscribing for RPC...");
        if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
          Serial.println("Failed to subscribe for RPC");
          return;
        }

        if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
          Serial.println("Failed to subscribe for shared attribute updates");
          return;
        }

        Serial.println("Subscribe done");

        if (!tb.Shared_Attributes_Request(attribute_shared_request_callback)) {
          Serial.println("Failed to request for shared attributes");
          return;
        }
      }

      // if (attributesChanged) {
      //   attributesChanged = false;
      //   tb.sendAttributeData(LED_STATE_ATTR, digitalRead(LED_PIN));
      // }

      // if (ledMode == 1 && millis() - previousStateChange > blinkingInterval) {
      //   previousStateChange = millis();
      //   digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      //   Serial.print("LED state changed to: ");
      //   Serial.println(!digitalRead(LED_PIN));
      // }

      if (millis() - previousDataSend > telemetrySendInterval) {
        previousDataSend = millis();

        dht20.read();
        
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

        if (xSemaphoreTake(xGlobMutex, portMAX_DELAY) == pdTRUE) {
          glob_temperature = temperature;
          glob_humidity = humidity;
          xSemaphoreGive(xGlobMutex);
        }

        if (isnan(temperature) || isnan(humidity)) {
          Serial.println("Failed to read from DHT20 sensor!");
        } 
        else {
          Serial.print("Temperature: ");
          Serial.print(temperature);
          Serial.print(" °C, Humidity: ");
          Serial.print(humidity);
          Serial.println(" %");

          tb.sendTelemetryData("temperature", temperature);
          tb.sendTelemetryData("humidity", humidity);
        }

        tb.sendAttributeData("rssi", WiFi.RSSI());
        tb.sendAttributeData("channel", WiFi.channel());
        tb.sendAttributeData("bssid", WiFi.BSSIDstr().c_str());
        tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
        tb.sendAttributeData("ssid", WiFi.SSID().c_str());
      }

      tb.loop();

      vTaskDelay(10);
    }
}