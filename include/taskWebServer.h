// Copyright (c) 2021 SQ9MDD Rysiek Labus
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <html.h>
#include <settings_woda.h>
#include <settings_domo.h>
#include <settings_telegram.h>
#include <settings_wifi.h>
#include <settings.h>
#include <EEPROM.h>
#include <version.h>

#define AP_SSID "Ed's Gardner AP"
#define AP_PASS "niceflowers"

extern ESP8266WebServer server;

void getJSON();
void handle_Index();
void restServerRouting();
void handleNotFound();
void connect_to_wifi();
void save_cfg();
void save_telegram_spiffs();