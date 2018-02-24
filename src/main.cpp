//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                                          ////
////  TTGO ESP8266 0.91 inch OLED demo w/ OTA                                                                 ////
////  requires U8g2 library                                                                                   ////
////  Select "NodeMCU 0.9 (ESP-12 Module)" board                                                              ////
////  More examples at http://simplestuffmatters.com                                                          ////
////  USB drivers: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers  ////
////                                                                                                          ////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ESP8266WiFi.h>  //For ESP8266
#include <ESP8266mDNS.h>  //For OTA
#include <WiFiUdp.h>      //For OTA
#include <ArduinoOTA.h>   //For OTA
#include <Arduino.h>
#include <U8g2lib.h>      // make sure to add U8g2 library and restart Arduino IDE  
#include <SPI.h>
#include <Wire.h>
#include "config.h"
#include "wifi.h"

#define OLED_SDA  2
#define OLED_SCL 14
#define OLED_RST  4
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);

const char *TXT_WELCOME  = "RATP v0.1";  // µ scroll this text from right to left
const char *TXT_HOSTNAME = "micro_ratp";  // scroll this text from right to left

const char *text = "cmpi_bus";  // scroll this text from right to left

//Necesary to make Arduino Software autodetect OTA device
WiFiServer TelnetServer(8266);

void setup_wifi() {
  delay(250);
  /*
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  */
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("   IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Configuring OTA device...");

  #if #TELNET
   TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device  
  #endif

  ArduinoOTA.onStart([]() {Serial.println("OTA starting...");});
  ArduinoOTA.onEnd([]() {Serial.println("OTA update finished!");Serial.println("Rebooting...");});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {Serial.printf("OTA in progress: %u%%\r\n", (progress / (total / 100)));});  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Wifi OK");
}

void setup_lcd() {
 u8g2.begin();
 u8g2.clearBuffer();          // clear the internal memory
 u8g2.setFont(u8g2_font_8x13B_mf); // choose a suitable font
 u8g2.drawStr(0,10,TXT_WELCOME);  // write something to the internal memory
 u8g2.sendBuffer();          // transfer internal memory to the display
}
 
void setup() {
  Serial.begin(9600);
  setup_lcd();
  setup_wifi();  
 }
 
void loop() {
  ArduinoOTA.handle();
  u8g2.drawStr(0,10,text);  // write something to the internal memory
  IPAddress myip= WiFi.localIP();
  String sFullip = String(myip[0]) + "." + myip[1] + "." + myip[2] + "." + myip[3];
  u8g2.drawStr(0,28,sFullip.c_str());  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  
}
