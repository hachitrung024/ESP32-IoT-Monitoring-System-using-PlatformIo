#ifndef __SENDER_WEBSERVER_H__
#define __SENDER_WEBSERVER_H__

#include <ArduinoJson.h>

#include "global.h"
#include "webserver.h"

void send_data_webserver_task(void *pvParameters);
#endif