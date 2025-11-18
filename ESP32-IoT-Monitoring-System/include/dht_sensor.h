#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "DHT.h"
#include "global.h"

#ifndef DHT_TYPE
#define DHT_TYPE DHT11
#endif

#define DHT_PIN 8

void dht_task(void * pvParameter);

#endif