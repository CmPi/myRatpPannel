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

#include <ESP8266WiFi.h> //For ESP8266
#if OTA_SUPPORT
#include <ESP8266mDNS.h> //For OTA
#include <WiFiUdp.h>     //For OTA
#include <ArduinoOTA.h>  //For
#endif

#include <Arduino.h>
#include <ArduinoJson.h>

#if OLED_SUPPORT
#include <U8g2lib.h> // make sure to add U8g2 library and restart Arduino IDE
#define OLED_SDA 2
#define OLED_SCL 14
#define OLED_RST 4
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA, OLED_RST);
#endif

#include <Bridge.h>
#include <HttpClient.h>
#include <WiFiClientSecure.h>

const char *hostUrl = "api-ratp.pierre-grimaud.fr";
const int httpsPort = 443;

const char *TXT_WELCOME = APP_NAME " v" APP_VERSION; // µ scroll this text from right to left
const char *TXT_HOSTNAME = "micro_ratp";            // scroll this text from right to left

const char *text = "cmpi_bus"; // scroll this text from right to left

char aOledRow[OLED_COLS_NUM];

int iColScan = 64;

const char *ssid = WIFI_1_SSID;
const char *password = WIFI_1_PASSWORD;

//Necesary to make Arduino Software autodetect OTA device
WiFiServer TelnetServer(8266);

void Scan()
{
  if (iColScan > OLED_COLS_NUM) // 112
    iColScan = 0;
  u8g2.drawStr(iColScan * OLED_COLS_WIDTH, 3 * OLED_ROWS_HEIGHT, " * "); // write something to the internal memory
  u8g2.sendBuffer();                                                     // transfer internal memory to the display
  iColScan = iColScan + 2;
}

void espInfo()
{
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  DEBUG_MSG("Flash real id:   %08X\n", ESP.getFlashChipId());
  DEBUG_MSG("Flash real size: %u\n\n", realSize);
  DEBUG_MSG("Flash ide  size: %u\n", ideSize);
  DEBUG_MSG("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
  DEBUG_MSG("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  if (ideSize != realSize)
  {
    DEBUG_MSG_P(PSTR("Flash Chip configuration wrong!\n"));
  }
  else
  {
    DEBUG_MSG_P(PSTR("Flash Chip configuration ok.\n"));
  }
}

void wifiSetup()
{
  WiFi.mode(WIFI_AP_STA);

#if DEBUG_SUPPORT
  DEBUG_MSG_P(PSTR("Connecting to "));
  DEBUG_MSG_P(PSTR(WIFI_1_SSID));
#endif

  // WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Scan();
    delay(500);
    DEBUG_MSG_P(PSTR("."));
  }

#if DEBUG_SUPPORT
  DEBUG_MSG_P(PSTR("   IP address: "));
  Serial.println(WiFi.localIP());
  DEBUG_MSG_P(PSTR("Configuring OTA device..."));
#endif

#if TELNET_SUPPORT
  TelnetServer.begin(); //Necesary to make Arduino Software autodetect OTA device
#endif

#if DEBUG_SUPPORT
  Serial.println("Wifi OK");
#endif
}

#if OLED_SUPPORT

void oledSetup()
{
  u8g2.begin();
  u8g2.clearBuffer();                             // clear the internal memory
  u8g2.setFont(OLED_FONT);                        // choose a suitable font -- u8g2_font_8x13B_mf
  u8g2.drawStr(0, OLED_ROWS_HEIGHT, TXT_WELCOME); // write something to the internal memory
  u8g2.sendBuffer();                              // transfer internal memory to the display
}

#endif

#if OTA_SUPPORT

void otaSetup()
{
  ArduinoOTA.onStart([]() { Serial.println("OTA starting..."); });
  ArduinoOTA.onEnd([]() {Serial.println("OTA update finished!");Serial.println("Rebooting..."); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Serial.printf("OTA in progress: %u%%\r\n", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

#endif
WiFiClientSecure client;

// Peut rester quasi dans l'état

String getJson( String sApi ) {
  String headers = "";
  String body = "";
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool gotResponse = false;
  long now;
  int iLevel = 0;

  char host[] = "api-ratp.pierre-grimaud.fr";

  if (client.connect(host, 443))
  {
    client.println("GET " + sApi + " HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("User-Agent: arduino/1.0");
    client.println("");

    now = millis();
    // checking the timeout
    while (millis() - now < 1500)
    {
      while (client.available())
      {
        char c = client.read();
        //Serial.print(c);
        if (c=='{')
         iLevel++;
        if (finishedHeaders)
        {
          if (iLevel>0) {
            body = body + c;
          }
        }
        else
        {
          if (currentLineIsBlank && c == '\n')
          {
            finishedHeaders = true;
          }
          else
          {
            headers = headers + c;
          }
        }

        if (c == '\n')
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          currentLineIsBlank = false;
        }

        if (c=='}')
         iLevel--;

        //marking we got a response
        gotResponse = true;
      } // while
    }
  }

  return body;
}

// Affiche un String sur l'écran OLED

void oledDraw( int iCol, int iRow, String sChaine ) {
  sChaine.toCharArray(aOledRow,OLED_COLS_NUM);
  u8g2.drawStr( iCol * OLED_COLS_WIDTH, iRow * OLED_ROWS_HEIGHT, aOledRow );
  u8g2.sendBuffer();  
}

void ratpTest()
{
  String body = "";
  String sDuree = "";

  body = getJson("/v3/schedules/bus/131/les+coquettes/R");
  if (body!="") {
    Serial.println(body);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(body);
        if (root.success())
        {
          if (root.containsKey("result"))
          {
            JsonObject &post = root["result"]["schedules"][0];
              if (post.containsKey("destination"))
            {
              sDuree = post["destination"].as<String>();
              Serial.println(sDuree);
              oledDraw( 0, 3, sDuree  );
          }
            if (post.containsKey("message"))
            {
              sDuree = post["message"].as<String>();
              Serial.println(sDuree);
              if (sDuree=="A l'approche")
               sDuree="appro";
              if (sDuree=="A l'arret")
               sDuree="arret";
              oledDraw( OLED_COLS_NUM-2*5 -2, 3, " "+ sDuree + " " );
          }
            JsonObject &post2 = root["result"]["schedules"][1];
            if (post2.containsKey("message"))
            {
              sDuree = post2["message"].as<String>();
              Serial.println(sDuree);
              oledDraw( OLED_COLS_NUM -1*5 -2, 3, " "+ sDuree + " " );
            }

          }
        }
        else
        {
          Serial.println("failed to parse JSON");
        }
      }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ; // do nothing until the serial monitor is opened
  DEBUG_MSG_P(PSTR("\n\nsetup()\n\n"));
#if OLED_SUPPORT
  oledSetup();
#endif
  wifiSetup();
#if OTA_SUPPORT
  otaSetup();
#endif
#if OLED_SUPPORT
  IPAddress myip = WiFi.localIP();
  String sFullip = String(myip[0]) + "." + myip[1] + "." + myip[2] + "." + myip[3];
  u8g2.drawStr(0, 3 * OLED_ROWS_HEIGHT, sFullip.c_str()); // write something to the internal memory
  u8g2.sendBuffer();                                      // transfer internal memory to the display
#endif
}

void wifiScan()
{
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
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}

void ratpLoop()
{
  ratpTest();
}

void loop()
{
#if OTA_SUPPORT
  ArduinoOTA.handle();
#endif
  ratpLoop();
  delay(45000);
}
