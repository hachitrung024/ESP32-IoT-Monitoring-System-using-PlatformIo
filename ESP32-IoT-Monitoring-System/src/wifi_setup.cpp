#include "wifi_setup.h"

void startAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(String(AP_SSID), String(AP_PASSWORD));
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    if (xSemaphoreTake(xApModeMutex, portMAX_DELAY) == pdTRUE) {
        is_ap_mode = true;
        xSemaphoreGive(xApModeMutex);
    }
    if (xSemaphoreTake(xConnectingMutex, portMAX_DELAY) == pdTRUE) {
        is_connecting = false;
        xSemaphoreGive(xConnectingMutex);
    }
    xSemaphoreGive(xNeoUpdateSemaphore);
}

void startSTA()
{
    if (WIFI_SSID.isEmpty())
    {
        Serial.println("Info: Reconnect Wifi failed, SSID is empty");
        return;
    }

    WiFi.mode(WIFI_STA);

    if (xSemaphoreTake(xApModeMutex, portMAX_DELAY) == pdTRUE) {
        is_ap_mode = false;
        xSemaphoreGive(xApModeMutex);
    }
    if (xSemaphoreTake(xConnectingMutex, portMAX_DELAY) == pdTRUE) {
        is_connecting = true;
        xSemaphoreGive(xConnectingMutex);
    }
    xSemaphoreGive(xNeoUpdateSemaphore);

    Serial.print("Info: Connecting to WiFi...");
    if (WIFI_PASSWORD.isEmpty())
    {
        WiFi.begin(WIFI_SSID.c_str());
    }
    else
    {
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    //Internet access
    Serial.println("");
    Serial.println("Info: Connected to WiFi");
    Serial.println("WIFI_SSID = " + String(WIFI_SSID));
    Serial.println("WIFI_PASSWORD = " + String(WIFI_PASSWORD));

    if (xSemaphoreTake(xWifiConnectedMutex, portMAX_DELAY) == pdTRUE) {
        is_wifi_connected = true;
        xSemaphoreGive(xWifiConnectedMutex);
    }
    if (xSemaphoreTake(xApModeMutex, portMAX_DELAY) == pdTRUE) {
        is_ap_mode = false;
        xSemaphoreGive(xApModeMutex);
    }
    if (xSemaphoreTake(xConnectingMutex, portMAX_DELAY) == pdTRUE) {
        is_connecting = false;
        xSemaphoreGive(xConnectingMutex);
    }
    xSemaphoreGive(xNeoUpdateSemaphore);

    // Begin signal that internet is available, connect to core IoT
    xSemaphoreGive(xBinarySemaphoreInternet);
}

bool reconnectWiFi()
{
    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED)
    {
        return true;
    }

    // Not connected, try to reconnect
    if (xSemaphoreTake(xWifiConnectedMutex, portMAX_DELAY) == pdTRUE) {
        is_wifi_connected = false;
        xSemaphoreGive(xWifiConnectedMutex);
    }
    xSemaphoreGive(xNeoUpdateSemaphore);

    startSTA();
    return false;
}
