#ifndef __CORE_IOT_H__
#define __CORE_IOT_H__

#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>

#include "DHT20.h"
#include "Wire.h"
#include "global.h"

#define LED_PIN GPIO_NUM_45
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12

constexpr char TOKEN[] = "w2l8e2sfjrdhzbd5u6in";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;

void coreiot_task(void *pvParameters);

#endif

