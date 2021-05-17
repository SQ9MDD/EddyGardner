// Copyright (c) 2021 SQ9MDD Rysiek Labus
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>
#include <taskWebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DallasTemperature.h>                                  // biblioteka do komunikacji z czujnikami DS18B20
#include <OneWire.h>                                            // biblioteka do komunikacji z czujnikami one wire
#include <EEPROM.h>
#include <LittleFS.h>

#define bi1               D1                                    // water tank low lvl
#define bi3               D2                                    // push button 
#define bo1               D3                                    // water pump 1
#define ai1               A0                                    // soil moisture
#define ai2               D4                                    // temperature (DS18B20)

OneWire oneWire(ai2);
DallasTemperature sensors(&oneWire);

String www_pass = "root";
String gardner_name = "Eddy Gardner";
String wifi_config_file = "/wifi_conf.txt";
String config_file = "/config.txt";
String wifi_ssid = "";
String wifi_pass = "";
String telegram_config_file = "/telegram_conf.txt";
String telegram_bot_api_key;
String telegram_chat_id;
String telegram_low_lvl_txt;
String telegram_low_lvl_val;
boolean last_telegram_active = false;
boolean telegram_active = false;
boolean telegram_alarm_sent = false;
boolean telegram_dealarm_sent = true;
boolean spiffsActive = false;
boolean bo_state = false;
boolean low_lvl = false;
int moisture_low_limit = 20;
int moisture_hi_limit = 90;
int moisture = 0;
int moisture_set = 65;                                          // default moisture setpoint
int water_lvl_max = 10000;                                      // water level maximu in ml.
int water_lvl = water_lvl_max;
int domoti_IP_1 = 192;
int domoti_IP_2 = 168;
int domoti_IP_3 = 0;
int domoti_IP_4 = 1;
int domoti_PORT = 80;
int send_interval = 5;
boolean domoti_on = false;
int idx_moisture_sensor = 1;
int idx_temp_sensor = 2;
int idx_water_tank = 3;
int idx_water_pump = 4;

float temperature;
unsigned long last_tick;
unsigned long domoticz_interval;
unsigned long last_data_send;
unsigned long feed_interval = 300000;
unsigned long bo1_start_time;
unsigned long last_moisture_send;                               // moisture sending timer
unsigned long impulse_ml = 250;                                 // one impulse amount 
unsigned long impulse_time = 67;                                // impulse time

// type_sensor enum: 1-temperature, 2- moisture, 3-switch
void send_domoticz(int type_sensor, float value_sensor, int idx){
  String url = "http://" + String(domoti_IP_1) + "." + String(domoti_IP_2) + "." + String(domoti_IP_3) + "." + String(domoti_IP_4) + ":" +String(domoti_PORT) + "/json.htm";
  if(type_sensor == 1){
    url +=  "?type=command&param=udevice&idx=" + String(idx) + "&nvalue=0&svalue=" + String(value_sensor,1);
  }
  if(type_sensor == 2){
    url += "?type=command&param=udevice&idx=" + String(idx) + "&nvalue=" + String(value_sensor,1);
  } 
  if(type_sensor == 3){
    if(value_sensor == 1.0){
      url += "?type=command&param=switchlight&idx=" + String(idx) + "&switchcmd=On";
    }else{
      url += "?type=command&param=switchlight&idx=" + String(idx) + "&switchcmd=Off";
    }
  }
  WiFiClient client;
  HTTPClient http;
  http.begin(client,url);
  http.GET();
  // dopisac obslugę błędów
  http.end();
  Serial.println(url);
}

void send_telegram(String txt_msg){
  String url = "https://api.telegram.org/bot" + telegram_bot_api_key + "/sendMessage?chat_id=" + telegram_chat_id + "&text=" + txt_msg;
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  if (https.begin(*client, url)) {  // HTTPS
    https.GET();
  }  
  Serial.println(url);  
}

void BO_SET(){
  if(low_lvl == false && moisture > moisture_low_limit && moisture < moisture_set){
    bo_state = true; 
    bo1_start_time = millis(); 
    delay(100);
    digitalWrite(bo1, LOW);
    water_lvl = water_lvl - impulse_ml;
    if(domoti_on){
      send_domoticz(3, 1.0, idx_water_pump);
    }
  }else{
    delay(200);
  }
}

void BO_RESET() {
  bo_state = false;
  digitalWrite(bo1, HIGH);
  if(domoti_on){
    send_domoticz(3, 0.0, idx_water_pump);
  }  
  delay(100);
}

void read_global(){
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }
  File file = LittleFS.open(config_file,"r");
  if(!file){
    Serial.println("error opening file");
  }else{
    String s;
    int line = 0;
    while (file.position()<file.size()){
      s = file.readStringUntil('\n');
      s.trim();
      if(line == 0){  // nazwa obiektu
        gardner_name = s.c_str();
      }  
      if(line == 1){  // hasło do www
        www_pass = s.c_str();
      }       
      line++;
    }
  file.close();    
  }
}

void read_wifi_spiffs(){
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }
  File file = LittleFS.open(wifi_config_file,"r");
  if(!file){
    Serial.println("error opening file");
  }
    String s;
    int line = 0;
    while (file.position()<file.size()){
      s = file.readStringUntil('\n');
      s.trim();
      if(line == 0){  //bot api
        wifi_ssid = s.c_str();
      }
      if(line == 1){  //chat id
        wifi_pass = s.c_str();
      }    
      line++;
    }
  file.close();
}

void read_telegram_spiffs(){
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }
  File file = LittleFS.open(telegram_config_file,"r");
  if(!file){
    Serial.println("error opening file");
  }
    String s;
    int line = 0;
    while (file.position()<file.size()){
      s = file.readStringUntil('\n');
      s.trim();
      if(line == 0){  //bot api
        telegram_bot_api_key = s.c_str();
      }
      if(line == 1){  //chat id
        telegram_chat_id = s.c_str();
      }
      if(line == 2){  // tekst alarmu niskiego poziomu wody
        telegram_low_lvl_txt = s.c_str();
      }
      if(line == 3){  // tekst alarmu niskiego poziomu wody
        telegram_low_lvl_val = s.c_str();
      }  
      if(line == 4){
          telegram_active = s.toInt();
      }    
      line++;
    }
  file.close();
}

void setup() {
  Serial.begin(115200);   
  pinMode(ai1,INPUT);
  pinMode(bo1,OUTPUT);
  digitalWrite(bo1,HIGH);
  pinMode(bi1,INPUT_PULLUP);
  pinMode(bi3,INPUT_PULLUP);
  delay(1000);
  Serial.println("\n\n");
  Serial.println("read config..."); 
  // read domoticz settings from spiffs
  read_telegram_spiffs();
  read_wifi_spiffs();
  read_global();
  delay(2000);
  // read settings from eeprom
  EEPROM.begin(1024);
  delay(10);
  if(EEPROM.read(0) == 0xFE){                           // znacznik zapisu
    moisture_set = EEPROM.read(1);  
    impulse_ml = EEPROM.read(2);
    feed_interval = u_long(EEPROM.read(3) * 1000 * 60);
    water_lvl_max = (EEPROM.read(4) * 1000);
    water_lvl = water_lvl_max;
  }
  if(EEPROM.read(16) == 0xFE){                          // znacznik zapisu
    domoti_IP_1 = EEPROM.read(5);
    domoti_IP_2 = EEPROM.read(6);
    domoti_IP_3 = EEPROM.read(7);
    domoti_IP_4 = EEPROM.read(8);
    domoti_PORT = EEPROM.read(9);
    send_interval = EEPROM.read(10);
    domoti_on = EEPROM.read(11);
    idx_moisture_sensor = EEPROM.read(12);
    idx_temp_sensor = EEPROM.read(13);
    idx_water_tank = EEPROM.read(14);
    idx_water_pump = EEPROM.read(15);    
  }
  domoticz_interval = (send_interval * 60 * 1000);
  Serial.println("booting...");
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  connect_to_wifi();
  restServerRouting();
  server.onNotFound(handleNotFound);
  server.begin();
  ArduinoOTA.setPassword(INI_UPLOAD_PASS);
  ArduinoOTA.onStart([](){
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH){
			type = "sketch";
		}else{
			type = "filesystem";
		}
	});
  ArduinoOTA.begin();
  int moisture_raw = analogRead(ai1);
  moisture = map(moisture_raw,308,760,100,0);
  Serial.println("booting done, enjoy\n"); 
  Serial.println("www username is: root");
  Serial.println("www password is: " + String(www_pass) + "\n");  
}

void loop(){
  ArduinoOTA.handle();
  server.handleClient();

  // wylaczenie pompki po przepompowaniu dawki
  if(bo_state == true && (millis() - bo1_start_time) >= (impulse_time * impulse_ml)){
    BO_RESET();
  }

  // test plywaka dolnego wykrywanie braku wody ustawia zmienną water_lvl na 0%
  // zwarcie na pływaku jednorazowo ustawia zmienną water_lvl na 100% (napełnienie zbiornika)
  if(digitalRead(bi1) != low_lvl){
    delay(50);
    if(digitalRead(bi1) != low_lvl){
      low_lvl = digitalRead(bi1); 
      if(low_lvl){
        water_lvl = 0;
      }
      if(low_lvl == false && water_lvl == 0){
        water_lvl = water_lvl_max;
      }
    }
  }

  // przycisk
  if(digitalRead(bi3) == LOW){
    delay(10);
    if(digitalRead(bi3) == LOW){
      BO_SET();
    }
  } 

  // testowanie wilgotnosci raz na 5 sec
  if(millis() - last_tick > 5000){
    int moisture_raw = analogRead(ai1);
    moisture = (((moisture * 9) + map(moisture_raw,308,760,100,0)) / 10); // wygładzanie odczytów
    moisture = constrain(moisture,0,100);
    sensors.requestTemperatures();
    float tmp_mesure = sensors.getTempCByIndex(0);
    if(tmp_mesure > -15.0){ // podczas pracy pompy pojawiają się anomalie odczytów wiec pomijam błędne pomiary
      temperature = (((temperature * 9 ) + tmp_mesure) / 10); // wygładzanie odczytów
    }
    if(water_lvl < telegram_low_lvl_val.toInt() && telegram_alarm_sent == false && telegram_active){
      send_telegram(gardner_name + ": " + telegram_low_lvl_txt + " ALARM"); // <-------------- dodac nazwe systemu w alarmie
      telegram_alarm_sent = true;
      telegram_dealarm_sent = false;
    }
    if(water_lvl > telegram_low_lvl_val.toInt() && telegram_dealarm_sent == false && telegram_active){
      send_telegram(gardner_name + ": " + telegram_low_lvl_txt + " KONIEC ALARMU");
      telegram_dealarm_sent = true;
      telegram_alarm_sent = false;
    }

    // wysłanie wiadomości po aktywacji i deaktywacji powiadomien.
    if(last_telegram_active != telegram_active){
      if(telegram_active){
        send_telegram("Aktywacja powiadomień na " + gardner_name);
      }else{
        send_telegram("Deaktywacja powiadomień na " + gardner_name);
        telegram_alarm_sent = false;
      }
      last_telegram_active = telegram_active;  
    }
    // debug
    int water_percent = map(water_lvl,0,water_lvl_max,0,100);
    Serial.println(String(millis()) + ";" + String(water_lvl) + ";" + String(water_lvl_max) + ";" + String(water_percent));
    last_tick = millis();
  }

  // domoticz integration
  domoticz_interval = u_long(send_interval * 60 * 1000);
  if((millis() - last_data_send > domoticz_interval) && domoti_on){
    int water_percent = map(water_lvl,0,water_lvl_max,0,100);
    water_percent = constrain(water_percent,0,100);
    send_domoticz(1, temperature, idx_temp_sensor);
    send_domoticz(2, moisture, idx_moisture_sensor);
    send_domoticz(1, water_percent, idx_water_tank);
    if(bo_state){
      send_domoticz(3, 1.0, idx_water_pump);
    }else{
      send_domoticz(3, 0.1, idx_water_pump);
    }
    last_data_send = millis();
  }

  // raz na x minut podlewanie automatyczne
  if(bo_state == false && (millis() - last_moisture_send) > feed_interval){
    if(moisture < moisture_set){
      BO_SET();
    }
    last_moisture_send = millis(); 
  }
}