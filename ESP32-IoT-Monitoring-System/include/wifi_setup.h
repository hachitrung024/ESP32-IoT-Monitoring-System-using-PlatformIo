#ifndef __TASK_WIFI_H__
#define __TASK_WIFI_H__

#include <WiFi.h>
#include <check_info_file.h>
#include <webserver.h>

extern bool Wifi_reconnect();
extern void startAP();

#endif