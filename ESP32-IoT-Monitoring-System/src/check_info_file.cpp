#include "check_info_file.h"

void loadInfoFile()
{
  File file = LittleFS.open("/info.dat", "r");
  if (!file)
  {
    return;
  }
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.print(F("Error: deserializeJson() failed!"));
  }
  else
  {
    WIFI_SSID = strdup(doc["WIFI_SSID"]);
    WIFI_PASSWORD = strdup(doc["WIFI_PASS"]);
    CORE_IOT_TOKEN = strdup(doc["CORE_IOT_TOKEN"]);
    CORE_IOT_SERVER = strdup(doc["CORE_IOT_SERVER"]);
    CORE_IOT_PORT = strdup(doc["CORE_IOT_PORT"]);
  }
  file.close();
}

void deleteInfoFile()
{
  if (LittleFS.exists("/info.dat"))
  {
    LittleFS.remove("/info.dat");
  }
  ESP.restart();
}

void saveInfoFile(String wifi_ssid, String wifi_pass, String CORE_IOT_TOKEN, String CORE_IOT_SERVER, String CORE_IOT_PORT)
{
  Serial.println(wifi_ssid);
  Serial.println(wifi_pass);

  DynamicJsonDocument doc(4096);
  doc["WIFI_SSID"] = wifi_ssid;
  doc["WIFI_PASS"] = wifi_pass;
  doc["CORE_IOT_TOKEN"] = CORE_IOT_TOKEN;
  doc["CORE_IOT_SERVER"] = CORE_IOT_SERVER;
  doc["CORE_IOT_PORT"] = CORE_IOT_PORT;

  File configFile = LittleFS.open("/info.dat", "w");
  if (configFile)
  {
    serializeJson(doc, configFile);
    configFile.close();
  }
  else
  {
    Serial.println("Error: Unable to save the configuration!");
  }
  ESP.restart();
};

bool checkInfoFile(bool check)
{
  if (!check)
  {
    if (!LittleFS.begin(true))
    {
      Serial.println("Error: LittleFS startup failed!");
      return false;
    }
    loadInfoFile();
  }
  
  if (WIFI_SSID.isEmpty() && WIFI_PASSWORD.isEmpty())
  {
    if (!check)
    {
      startAP();
    }
    return false;
  }
  return true;
}