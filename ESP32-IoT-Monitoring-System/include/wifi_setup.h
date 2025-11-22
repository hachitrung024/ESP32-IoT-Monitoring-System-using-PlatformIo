#ifndef __WIFI_SETUP_H__
#define __WIFI_SETUP_H__

#include <WiFi.h>
#include <check_info_file.h>
#include <webserver.h>

extern bool reconnectWiFi();
extern void startAP();

#endif