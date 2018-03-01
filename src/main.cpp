//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                                          ////
////  TTGO ESP8266 0.91 inch OLED demo w/ OTA                                                                 ////
////  requires U8g2 library                                                                                   ////
////  Select "NodeMCU 0.9 (ESP-12 Module)" board                                                              ////
////  More examples at http://simplestuffmatters.com                                                          ////
////  USB drivers: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers  ////
////                                                                                                          ////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "config/all.h"

#include <ESP8266WiFi.h>  //For ESP8266
#include <ESP8266mDNS.h>  //For OTA
#include <WiFiUdp.h>      //For OTA
#include <ArduinoOTA.h>   //For OTA
#include <Arduino.h>
#include <U8g2lib.h>      // make sure to add U8g2 library and restart Arduino IDE  
#include <SPI.h>
#include <Wire.h>
#include <Bridge.h>
#include <HttpClient.h>

#include <ArduinoJson.h>







#define OLED_SDA  2
#define OLED_SCL 14
#define OLED_RST  4
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);

const char *TXT_WELCOME  = "RATP v0.1";  // µ scroll this text from right to left
const char *TXT_HOSTNAME = "micro_ratp";  // scroll this text from right to left

const char *text = "cmpi_bus";  // scroll this text from right to left

int iColScan = 64;


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;


//Necesary to make Arduino Software autodetect OTA device
WiFiServer TelnetServer(8266);

void Scan() {
 if (iColScan>96) // 112
  iColScan = 32;
 u8g2.drawStr(iColScan,16," * ");  // write something to the internal memory
 u8g2.sendBuffer();          // transfer internal memory to the display
 iColScan=iColScan+2;  
}

void info_esp() {
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
  Serial.printf("Flash real size: %u\n\n", realSize);
  Serial.printf("Flash ide  size: %u\n", ideSize);
  Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
  Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  if(ideSize != realSize) {
    Serial.println("Flash Chip configuration wrong!\n");
  } else {
    Serial.println("Flash Chip configuration ok.\n");    
  }
}


void wifiSetup() {

  WiFi.mode(WIFI_AP_STA);

  #if DEBUG_SERIAL_SUPPORT
   Serial.print("Connecting to ");
   Serial.println(WIFI_SSID);
  #endif

 // WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin( ssid, password );
  while (WiFi.status() != WL_CONNECTED) {
    Scan();
    delay(500);
    Serial.print(".");
  }
  
  #if DEBUG_SERIAL_SUPPORT
   Serial.print("   IP address: ");
   Serial.println(WiFi.localIP());
   Serial.print("Configuring OTA device...");
  #endif

  #if TELNET_SUPPORT
   TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device  
  #endif

  

  #if DEBUG_SERIAL_SUPPORT  
   Serial.println("Wifi OK");
  #endif 
}


#if OLED_SUPPORT

void setup_oled() {
 u8g2.begin();
 u8g2.clearBuffer();          // clear the internal memory
 u8g2.setFont(u8g2_font_5x8_mf); // choose a suitable font -- u8g2_font_8x13B_mf
 u8g2.drawStr(0,7,TXT_WELCOME);  // write something to the internal memory
 u8g2.sendBuffer();          // transfer internal memory to the display
}

#endif

#if OTA_SUPPORT

void otaSetup(){
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
}

#endif
 
void setup() {
  DEBUG_MSG_P(PSTR("\n\nsetup()\n\n"));
  Serial.begin(9600);
  while (!Serial);     // do nothing until the serial monitor is opened

#if OLED_SUPPORT
  setup_oled();
#endif

  wifiSetup();

#if OTA_SUPPORT
  otaSetup();
#endif
}

void wifiScan(){
  

  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

}
 
void loop() {
  ArduinoOTA.handle();
  u8g2.drawStr(0,0,text);  // write something to the internal memory
  IPAddress myip= WiFi.localIP();
  String sFullip = String(myip[0]) + "." + myip[1] + "." + myip[2] + "." + myip[3];
  u8g2.drawStr(0,16,sFullip.c_str());  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  
}
