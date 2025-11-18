#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "DHT20.h"
#include "global.h"

#define DHT_PIN 8

void dht_task(void * pvParameter);

#endif