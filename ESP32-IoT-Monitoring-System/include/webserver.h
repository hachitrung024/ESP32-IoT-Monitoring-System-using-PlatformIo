#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <AsyncTCP.h>
#include <ArduinoJson.h>
// #include <ElegantOTA.h>
#include <handler_websocket.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;

extern bool webserver_isrunning;

void stopWebserver();
void reconnectWebserver();
void sendDataWebserver(String data);

#endif