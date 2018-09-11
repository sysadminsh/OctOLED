/**
   OctOLED - remote standalone status mini display for OctoPrint
   Project comes with great STL for 3D Print standing
   See more on GitHub: https://github.com/sysadminsh/OctOLED
   (C) 2018 SysAdmin.sh Jakub Furman
*/

// DO NOT TOUCH INCLUDE ZONE
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#define USE_SERIAL Serial

// VARIABLES TO CHANGE //
const char *ssid         = "WIFI SSID";
const char *password     = "WIFI PASSWORD";

// You can use normal url (without ending slash)
// Also you can provide Http Basic Authorization data if needed in that way: http://login:password@host
const String octoprint_url = "http://OCTOPRINT ADDRESS";
// OctoPrint API Key (read-only is enough)
const String octoprint_key = "OCTOPRINT API KEY";

const unsigned short delay_printing = 1000; // Refresh delay when printer is in Printing or Paused state [ms]
const unsigned short delay_nonprinting = 5000; // Refresh delay when printer is in Ready state [ms]
const unsigned short delay_error = 10000; // Refresh delay when there is HTTP error [ms]

const unsigned short contrast_printing = 20; // OLED contrast when printing or paused [1-255]
const unsigned short contrast_idle = 1; // OLED contrast when idling [1-255] (after half of idle_timeout it will be 2x darker) 
const unsigned short idle_timeout = 60*10; // Timeout when disable display after printer idle start [s]

const bool debug = false; // Set true to enable Serial port outputs

// BELOW DO NOT TOUCH AGAIN //
StaticJsonDocument<1200> doc;
SSD1306Wire  display(0x3c, D2, D1);
HTTPClient http;

float bed_act;
int bed_target;
float tool_act;
int tool_target;
float completion;
double eta;
int etaH;
int etaM;
bool printing;
bool paused;
bool idle = false;
String statusbar;
unsigned long idlestarttime;

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.clear();

  display.setFont(ArialMT_Plain_10);

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 4, "Connecting network:");
  display.drawString(64, 16, ssid);
  display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
  display.display();

  if(debug) {
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin ( ssid, password );

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 10 );
  }
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    // Ask about printer informations
    http.begin(octoprint_url+"/api/printer?history=false&limit=2&apikey="+octoprint_key);
    int httpCode = http.GET();

    if (httpCode > 0) {
  
      if(debug) {
        USE_SERIAL.printf("[HTTP] /api/printer ... code: %d\n", httpCode);
      }
      
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // additional debug possible here: USE_SERIAL.println(payload);
        
        // Decode json payload and set tool and bed variables
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          if(debug) {
            USE_SERIAL.print(F("deserializeJson() failed: "));
            USE_SERIAL.println(error.c_str());
          }
          display.clear();
          display.setFont(ArialMT_Plain_10);
          display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.drawString(64, 4, "Connection Error!");
          display.drawString(64, 16, "OctoPrint responded");
          display.drawString(64, 28, "JSON error");
          display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
          display.display();
          http.end();
          delay(delay_error);
          return;
        }
        
        JsonObject root = doc.as<JsonObject>();
        bed_act = root["temperature"]["bed"]["actual"];
        bed_target = root["temperature"]["bed"]["target"];
        tool_act = root["temperature"]["tool0"]["actual"];
        tool_target = root["temperature"]["tool0"]["target"];
        printing = root["state"]["flags"]["printing"];
        paused = root["state"]["flags"]["paused"];
        
      } else {
          if(debug) {
            USE_SERIAL.printf("[HTTP] /api/printer ... soft failed, error: %s\n", http.getString().c_str());
          }
          display.clear();
          display.setFont(ArialMT_Plain_10);
          display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.drawString(64, 4, "Connection Error!");
          display.drawString(64, 16, "OctoPrint responded");
          display.drawString(64, 28, http.getString());
          display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
          display.display();
          http.end();
          delay(delay_error);
          return;
      }
    } else {
      if(debug) {
        USE_SERIAL.printf("[HTTP] /api/printer ... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 4, "Connection Error!");
      display.drawString(64, 16, "OctoPrint responded");
      display.drawString(64, 28, http.errorToString(httpCode));
      display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
      display.display();
      http.end();
      delay(delay_error);
      return;
  }

    http.end();
    delay(200);

    // Ask about current job progress
    http.begin(octoprint_url+"/api/job?apikey="+octoprint_key);

    httpCode = http.GET();
    if (httpCode > 0) {
      if(debug) {
        USE_SERIAL.printf("[HTTP] /api/job... code: %d\n", httpCode);
      }

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // additional debug possible here: USE_SERIAL.println(payload);
        
        // Decode json payload and set progress variables
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          if(debug) {
            USE_SERIAL.print(F("deserializeJson() failed: "));
            USE_SERIAL.println(error.c_str());
          }
          display.clear();
          display.setFont(ArialMT_Plain_10);
          display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.drawString(64, 4, "Connection Error!");
          display.drawString(64, 16, "OctoPrint responded");
          display.drawString(64, 28, "JSON error");
          display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
          display.display();
          http.end();
          delay(delay_error);
          return;
        }
        
        JsonObject root = doc.as<JsonObject>();
        completion = root["progress"]["completion"];
        eta = root["progress"]["printTimeLeft"];
        // Calculate ETA to the hours and minutes
        eta = eta/60;
        etaH = (int) eta/60;
        etaM;
        etaM = (int) eta % 60;
      } else {
        if(debug) {
          USE_SERIAL.printf("[HTTP] /api/job ... soft failed, error: %s\n", http.getString().c_str());
        }
        
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 4, "Connection Error!");
        display.drawString(64, 16, "OctoPrint responded");
        display.drawString(64, 28, http.getString());
        display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
        display.display();
        http.end();
        delay(delay_error);
        return;
      }
    } else {
      if(debug) {
        USE_SERIAL.printf("[HTTP] /api/job ... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 4, "Connection Error!");
      display.drawString(64, 16, "OctoPrint responded");
      display.drawString(64, 28, http.errorToString(httpCode));
      display.drawString(64, 50, "OctOLED (C) SysAdmin.sh");
      display.display();
      http.end();
      delay(delay_error);
      return;
    }

    http.end();

    // Displayng prepared informations
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
  
    display.drawString(0, 0, "T: "+(String) tool_act +" / " + (String) tool_target);
    display.drawString(0, 17, "H: "+(String) bed_act +" / " + (String) bed_target);
    
    if(printing || paused) { // Version with status bar
      if(idle) {
        idle = false;
        idlestarttime = 0;
        display.displayOn();
      }
      display.drawProgressBar(0, 36, 120, 10, completion);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setContrast(contrast_printing,241,64);
      statusbar = String(completion) + "% / ";
      // Leading zeros adding
      if(etaH <10) {
        statusbar = statusbar + "0"+ etaH;
      } else {
        statusbar = statusbar + etaH;
      }
      statusbar = statusbar + ":";
      if(etaM < 10) {
        statusbar = statusbar + "0" + etaM;
      } else {
        statusbar = statusbar + etaM;
      }
      display.drawString(64, 48, statusbar);
      
      display.display();
      delay(delay_printing);
    } else { // Or without progress bar when non printing
      if(!idle) {
        idle = true;
        idlestarttime = millis();
      }

      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 45, "OctOLED Ready");
      
      if((millis()-idlestarttime)/1000 > idle_timeout) {
        display.displayOff();
      } else if ((millis()-idlestarttime)/1000 > idle_timeout/2) {
        display.setContrast(contrast_idle,5,0);
        display.display();
      } else {
        display.setContrast(contrast_idle,241,64); // 241,64
        display.display();
      }

      delay(delay_nonprinting);
    }
  }
}
