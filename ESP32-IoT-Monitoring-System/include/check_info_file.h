#ifndef __CHECK_INFO_FILE_H__
#define __CHECK_INFO_FILE_H__

#include <ArduinoJson.h>
#include "LittleFS.h"
#include "global.h"
#include "wifi_setup.h"

bool checkInfoFile(bool check);
void loadInfoFile();
void deleteInfoFile();
void saveInfoFile(String WIFI_SSID, String WIFI_PASS, String CORE_IOT_TOKEN, String CORE_IOT_SERVER, String CORE_IOT_PORT);

#endif