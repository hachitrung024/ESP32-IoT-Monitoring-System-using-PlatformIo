#include <handler_websocket.h>

void handleWebSocketMessage(String message)
{
    Serial.println(message);
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        Serial.println("Error: JSON parse error!");
        return;
    }
    JsonObject value = doc["value"];
    if (doc["page"] == "device")
    {
        if (!value.containsKey("gpio") || !value.containsKey("status"))
        {
            Serial.println("Error: JSON missing gpio or status information!");
            return;
        }

        int gpio = value["gpio"];
        String status = value["status"].as<String>();

        Serial.printf("Info: Control GPIO %d → %s\n", gpio, status.c_str());
        pinMode(gpio, OUTPUT);
        if (status.equalsIgnoreCase("ON"))
        {
            digitalWrite(gpio, HIGH);
            Serial.printf("Info: GPIO %d ON\n", gpio);
        }
        else if (status.equalsIgnoreCase("OFF"))
        {
            digitalWrite(gpio, LOW);
            Serial.printf("Info: GPIO %d OFF\n", gpio);
        }
    }
    else if (doc["page"] == "setting")
    {
        String WIFI_SSID = doc["value"]["ssid"].as<String>();
        String WIFI_PASS = doc["value"]["password"].as<String>();
        String CORE_IOT_TOKEN = doc["value"]["token"].as<String>();
        String CORE_IOT_SERVER = doc["value"]["server"].as<String>();
        String CORE_IOT_PORT = doc["value"]["port"].as<String>();

        Serial.println("Info: Received configuration from WebSocket:");
        Serial.println("SSID: " + WIFI_SSID);
        Serial.println("PASS: " + WIFI_PASS);
        Serial.println("TOKEN: " + CORE_IOT_TOKEN);
        Serial.println("SERVER: " + CORE_IOT_SERVER);
        Serial.println("PORT: " + CORE_IOT_PORT);

        // Call the function to save the configuration
        saveInfoFile(WIFI_SSID, WIFI_PASS, CORE_IOT_TOKEN, CORE_IOT_SERVER, CORE_IOT_PORT);

        // Respond to client (optional)
        String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
        ws.textAll(msg);
    }
}
