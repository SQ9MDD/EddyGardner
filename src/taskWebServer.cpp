// Copyright (c) 2021 SQ9MDD Rysiek Labus
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "taskWebServer.h"

ESP8266WebServer server(80);

extern String www_pass;
extern String gardner_name;
extern String config_file;
extern String wifi_config_file;
extern String wifi_ssid;
extern String wifi_pass;
extern String telegram_config_file;
extern String telegram_bot_api_key;
extern String telegram_chat_id;
extern String telegram_low_lvl_txt;
extern String telegram_low_lvl_val;
extern boolean telegram_active;
extern boolean spiffsActive;
extern boolean low_lvl;
extern unsigned long time_to_tick;
extern unsigned long impulse_ml;
extern unsigned long feed_interval;
extern int moisture;
extern int moisture_low_limit;
extern int moisture_hi_limit;
extern int moisture_set;
extern int water_lvl;
extern int water_lvl_max;
extern boolean bo_state;
extern float temperature;
extern int domoti_IP_1;
extern int domoti_IP_2;
extern int domoti_IP_3;
extern int domoti_IP_4;
extern int domoti_PORT;
extern int send_interval;
extern boolean domoti_on;
extern int idx_moisture_sensor;
extern int idx_temp_sensor;
extern int idx_water_tank;
extern int idx_water_pump;

void getJSON(){
  if(server.arg("page") == "home"){
    long water_percent = map(water_lvl, 0, water_lvl_max, 0, 100);
    water_percent = constrain(water_percent,0,100);
    String job_status;
    if(low_lvl == false){
      if(moisture <= moisture_low_limit){
        job_status = "\"<font color=red> Błąd umieszczenia czujnika wilgotności, praca niemożliwa. </font>\"";
      }else if(moisture > moisture_low_limit && moisture < moisture_set){
        job_status = "\"<font color=white> Normalna praca, niska wilgotność, tryb podlewania.</font>\"";
      }else if(moisture >= moisture_set && moisture < moisture_hi_limit){
        job_status = "\"<font color=white> Normalna praca, wilgotność w normie, oczekiwanie.</font>\"";
      }else if(moisture >= moisture_set && moisture >= moisture_hi_limit){
        job_status = "\"<font color=blue> Normalna praca, wilgotność powyżej normy, oczekiwanie.</font>\"";
      }else{
        job_status = "\"<font color=orange> Analizuję sytuację, dane są niejednoznaczne...</font>\"";
      }
    }else{
      job_status = "\"<font color=red> Brak wody w zbiorniku, praca niemożliwa. </font>\"";
    }
    String message = "{\"time\": ";
    message += millis();
    message += ", \"moisture\": ";
    message += moisture;
    message += ", \"temp\": ";
    message +=  String(temperature,1);  
    message += ", \"tank\": ";
    message += water_percent;  
    message += ", \"job_status\": ";
    message += job_status;
    message += ", \"pump\": ";
    if(bo_state){
      message += "\"PRACA\""; 
    }else{
      message += "\"STOP\""; 
    }
    message += "}";
    server.send(200, F("application/json"), message);    
  }
  else if(server.arg("page") == "water_set"){
    if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
    String message = "{\"time\": ";
    message += millis();
    message += ", \"moisture_set\": ";
    message += moisture_set;   
    message += ", \"tank_amount\": ";
    message += (water_lvl_max / 1000);  
    message += ", \"impulse_ml\": ";
    message += impulse_ml; 
    message += ", \"feed_interval\": ";
    message += (feed_interval / 1000 / 60); 
    message += "}";
    server.send(200, F("application/json"), message);    
  }
  else if(server.arg("page") == "domoti_set"){
    if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
    String message = "{\"time\": ";
    message += millis();
    message += ", \"domoti_IP_1\": ";
    message += domoti_IP_1;
    message += ", \"domoti_IP_2\": ";
    message += domoti_IP_2;  
    message += ", \"domoti_IP_3\": ";
    message += domoti_IP_3;  
    message += ", \"domoti_IP_4\": ";
    message += domoti_IP_4;  
    message += ", \"domoti_PORT\": ";
    message += domoti_PORT;    
    message += ", \"send_interval\": ";
    message += send_interval; 
    message += ", \"domoti_on\": ";
    message += domoti_on;      
    message += ", \"idx_moisture_sensor\": ";
    message += idx_moisture_sensor;   
    message += ", \"idx_temp_sensor\": ";
    message += idx_temp_sensor;   
    message += ", \"idx_water_tank\": ";
    message += idx_water_tank;   
    message += ", \"idx_water_pump\": ";
    message += idx_water_pump;     
    message += "}";
    server.send(200, F("application/json"), message);    
  }  
  else if(server.arg("page") == "telegram_set"){
    if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
    String message = "{\"time\": ";
    message += millis();
    message += ", \"telegram_bot_api_key\": ";
    message += "\"" + telegram_bot_api_key + "\"";  
    message += ", \"telegram_chat_id\": ";
    message += "\"" + telegram_chat_id + "\"";  
    message += ", \"telegram_low_lvl_txt\": ";
    message += "\"" + telegram_low_lvl_txt + "\""; 
    message += ", \"telegram_low_lvl_val\": ";
    message += "\"" + telegram_low_lvl_val + "\"";
    message += ", \"telegram_active\": ";
    message += telegram_active;     
    message += "}";
    server.send(200, F("application/json"), message);    
  } 
  else if(server.arg("page") == "wifi_set"){
    if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
    String message = "{\"time\": ";
    message += millis();
    message += ", \"wifi_ssid\": ";
    message += "\"" + wifi_ssid + "\""; 
    message += ", \"wifi_pass\": ";
    message += "\"" + wifi_pass + "\"";         
    message += "}";
    server.send(200, F("application/json"), message);    
  }
  else if(server.arg("page") == "global_set"){
    if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
    String message = "{\"time\": ";
    message += millis();
    message += ", \"gardner_name\": ";
    message += "\"" + gardner_name + "\"";   
    message += ", \"www_pass\": ";
    message += "\"" + www_pass + "\"";       
    message += "}";
    server.send(200, F("application/json"), message);    
  }
  else{
    String message = "{\"time\": ";
    message += millis();
    message += ", \"title\": ";
    message += "\"" + gardner_name + "\"";  
    message += ", \"version\": ";
    message += "\"" + String(VERSION_SHORT) + "<br> build: " + String(BUILD_NUMBER) + "\"";         
    message += "}";
    server.send(200, F("application/json"), message);     
  }
}

void save_settings(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  gardner_name = server.arg("gardner_name");
  www_pass = server.arg("www_pass");
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }  
  File file = LittleFS.open(config_file,"w");  
  file.print(gardner_name + "\n" + www_pass + "\n");
  file.close();  
  delay(2000);
  server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/settings\"></head><body><center><br><br><br><b>OK</body></html>");
}

void save_wifi(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  wifi_ssid = server.arg("wifi_ssid");
  wifi_pass = server.arg("wifi_pass");
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }  
  File file = LittleFS.open(wifi_config_file,"w"); 
  if(wifi_pass != ""){
    file.print(wifi_ssid + "\n" + wifi_pass + "\n");
    file.close();
    server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/set_wifi\"></head><body><center><br><br><br><b>OK RESTARTING</body></html>");
    delay(2000);
    ESP.restart();
  }else{
    server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/set_wifi\"></head><body><center><br><br><br><b>EMPTY PASSWORD</body></html>"); 
  }
}

void save_telegram_spiffs(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  telegram_bot_api_key = server.arg("telegram_bot_api_key");
  telegram_chat_id = server.arg("telegram_chat_id");
  telegram_low_lvl_txt = server.arg("telegram_low_lvl_txt");
  telegram_low_lvl_val = server.arg("telegram_low_lvl_val");
  telegram_active = server.arg("telegram_active").toInt();
  Serial.print(telegram_active);
  if (LittleFS.begin()){
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }  
  File file = LittleFS.open(telegram_config_file,"w");
  file.print(telegram_bot_api_key + "\n" + telegram_chat_id + "\n" + telegram_low_lvl_txt + "\n" + telegram_low_lvl_val  + "\n" + String(telegram_active) + "\n");
  file.close();
  delay(2000);
  server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/set_telegram\"></head><body><center><br><br><br><b>OK</body></html>");
}

void save_cfg(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  String moisture_set_str = server.arg("moisture_set");   moisture_set = moisture_set_str.toInt();
  String water_lvl_max_str = server.arg("tank_amount");   water_lvl_max = (water_lvl_max_str.toInt() * 1000);
  String impulse_ml_str = server.arg("impulse_ml");       impulse_ml = impulse_ml_str.toInt();
  String feed_interval_str = server.arg("feed_interval"); feed_interval = u_long(feed_interval_str.toInt() * 1000 * 60);
  
  //save settings to eeprom
  EEPROM.begin(1024);
  delay(30);
  EEPROM.write(0,0xFE);
  EEPROM.write(1,moisture_set);
  EEPROM.write(2,impulse_ml);
  EEPROM.write(3,int(feed_interval / 1000 / 60));
  EEPROM.write(4,(water_lvl_max / 1000));
  EEPROM.commit();
  EEPROM.end();
  server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/set_water\"></head><body><center><br><br><br><b>OK</body></html>");
}

void save_domoticz(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  domoti_IP_1 = server.arg("domoti_IP_1").toInt();
  domoti_IP_2 = server.arg("domoti_IP_2").toInt();
  domoti_IP_3 = server.arg("domoti_IP_3").toInt();
  domoti_IP_4 = server.arg("domoti_IP_4").toInt();
  domoti_PORT = server.arg("domoti_PORT").toInt();
  send_interval = server.arg("send_interval").toInt();
  domoti_on = server.arg("domoti_on").toInt();
  idx_moisture_sensor = server.arg("idx_moisture_sensor").toInt();
  idx_temp_sensor = server.arg("idx_temp_sensor").toInt();
  idx_water_tank = server.arg("idx_water_tank").toInt();
  idx_water_pump = server.arg("idx_water_pump").toInt();
  EEPROM.begin(1024);
  delay(30);
  EEPROM.write(5,domoti_IP_1);
  EEPROM.write(6,domoti_IP_2);
  EEPROM.write(7,domoti_IP_3);
  EEPROM.write(8,domoti_IP_4);
  EEPROM.write(9,domoti_PORT);
  EEPROM.write(10,send_interval);
  EEPROM.write(11,domoti_on);
  EEPROM.write(12,idx_moisture_sensor);
  EEPROM.write(13,idx_temp_sensor);
  EEPROM.write(14,idx_water_tank);
  EEPROM.write(15,idx_water_pump);
  EEPROM.write(16,0xFE);
  EEPROM.commit();
  EEPROM.end();
  server.send(200, F("text/html"), "<html><head><meta http-equiv=\"refresh\" content=\"1; url=/set_domo\"></head><body><center><br><br><br><b>OK</body></html>");
}

// main html site
void handle_Index(){
  server.send(200, "text/html", HTTP_HTML);
}

// water settings
void handle_set_water(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  server.send(200, "text/html", HTTP_WATER);
}

// domoticz settings
void handle_set_domo(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  server.send(200, "text/html", HTTP_DOMO);
}

// telegram settings
void handle_set_telegram(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  server.send(200, "text/html", HTTP_TELEGRAM);
}

// WiFi settings
void handle_set_wifi(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  server.send(200, "text/html", HTTP_WIFI);
}

// global settings
void handle_settings(){
  if(!server.authenticate("root", www_pass.c_str())) return server.requestAuthentication(DIGEST_AUTH, "login required for user root", "Authentication Failed");
  server.send(200, "text/html", HTTP_SETTINGS);
}

// Define routing
void restServerRouting(){
  server.on("/", handle_Index);
  server.on("/set_water", handle_set_water);
  server.on("/set_domo", handle_set_domo);
  server.on("/set_telegram", handle_set_telegram);
  server.on("/set_wifi", handle_set_wifi);
  server.on("/settings", handle_settings);
  server.on("/json.htm", getJSON);
  server.on("/save_cfg", save_cfg);
  server.on("/save_domoticz", save_domoticz);
  server.on("/save_telegram",save_telegram_spiffs);
  server.on("/save_wifi",save_wifi);
  server.on("/save_settings",save_settings);
}

// Manage not found URL
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void connect_to_wifi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);

  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to: "); 
  Serial.println(wifi_ssid);   
  int numbers_of_try = 0;
  while( WiFi.status() != WL_CONNECTED && numbers_of_try <= 25){
      delay(500);
      Serial.print(".");  
      numbers_of_try ++;      
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("\nWifi NOT Connected!");
    Serial.println("Activating AP HotSpot look for Ed's Gardner AP with pass: niceflowers");
    WiFi.disconnect(true);
    WiFi.softAP(AP_SSID, AP_PASS, 7);
    Serial.print("NodeMCU IP Address : ");   
    Serial.println(WiFi.softAPIP());      
  }else{
    Serial.println("\nWifi Connected Success!");
    Serial.print("NodeMCU IP Address : ");
    WiFi.hostname("EdGardner");
    Serial.println(WiFi.localIP());      
  }
}