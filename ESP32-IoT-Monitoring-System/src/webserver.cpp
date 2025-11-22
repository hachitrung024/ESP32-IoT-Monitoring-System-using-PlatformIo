#include "webserver.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool webserver_isrunning = false;

void sendDataWebserver(String data)
{
    if (ws.count() > 0)
    {
        ws.textAll(data); // Send to all connected clients
        Serial.println("Info: Sent data via WebSocket: " + data);
    }
    else
    {
        Serial.println("Info: No WebSocket clients are currently connected!");
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("Info: WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.printf("Info: WebSocket client #%u disconnected\n", client->id());
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->opcode == WS_TEXT)
        {
            String message;
            message += String((char *)data).substring(0, len);
            // parseJson(message, true);
            handleWebSocketMessage(message);
        }
    }
}

void connnectWSV()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "application/javascript"); });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });
    server.begin();
    // ElegantOTA.begin(&server);
    webserver_isrunning = true;
}

void stopWebserver()
{
    ws.closeAll();
    server.end();
    webserver_isrunning = false;
}

void reconnectWebserver()
{
    if (!webserver_isrunning)
    {
        connnectWSV();
    }
    // ElegantOTA.loop();
}
