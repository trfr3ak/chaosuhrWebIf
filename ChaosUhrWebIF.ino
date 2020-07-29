// ******************************************************************************
// *************            ChaosUhr mit Webinterface.              *************
// *************       Sketch basiert auf dem von Micha1986         *************
// *************       erweitert und ergänzt von Jonas Krug         *************
// ******************************************************************************

// Version 0.11

#include <FS.h>

#define FASTLED_FORCE_SOFTWARE_SPI
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <time.h>
#include <EEPROM.h>
#include <SPI.h>
#include "GifPlayer.h"
GifPlayer gifPlayer;
#include "Config.h"

// Fonts
#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>
#include "Fonts/apridot4pt7b.h"
#include "Fonts/spacerock_minecraft4pt7b.h"
#include "Fonts/art_bitco4pt7b.h"
#include "Fonts/burbank_big_2_03pt7b.h"
#include "Fonts/burbank_big_2_04pt7b.h"
#include "Fonts/to_infinity2pt7b.h"
#include "Fonts/to_infinity3pt7b.h"

#include <pgmspace.h>  // Needed to store stuff in Flash using PROGMEM



// Legacy Animationen

#include "legacy/Qbert.h"
#include "legacy/DigiDug.h"
#include "legacy/Face.h"
#include "legacy/Face2.h"
#include "legacy/Face3.h"
#include "legacy/Feuer.h"
#include "legacy/Drink.h"


//                  ***************************
//                  * Animation Einstellungen *
//                  ***************************
int UhrAnimation = 2;                             // Anzahl Durchläufe Datum/Uhrzeit
int WeatherAnimation = 20;                        // Anzahl Durchläufe Wetter
int FaceAnimation = 4;                            // Anzahl Durchläufe der Animation "blaues Gesicht" (ca. 5300ms pro Durchgang)
int Face2Animation = 10;                          // Anzahl Durchläufe der Animation "lila Gesicht" (ca. 140ms pro Durchgang)
int Face3Animation = 4;                           // Anzahl Durchläufe der Animation "rotes Gesicht" (ca. 1500ms pro Durchgang)
int FiresAnimation = 15;                          // Anzahl Durchläufe der Animation (ca. 480ms pro Durchgang)
int DrinkAnimation = 3;                           // Anzahl Durchläufe der Animation (ca. 3200ms pro Durchgang)
int DigiDugAnimation = 8;                         // Anzahl Durchläufe der Animation (ca. 500ms pro Durchgang)
int QbertAnimation = 5;                           // Anzahl Durchläufe der Animation (ca. 1000ms pro Durchgang)
int DemoReel1DugAnimation = 1100;                 // Anzahl Durchläufe der Animationen "rainbow, confetti, rainbowWithGlitter, sinelon"
int DemoReel2Animation = 510;                     // Anzahl Durchläufe der Animation "bpm, juggle" (ca. 10ms pro Durchgang)


//  **************************
//  *   Definiere WebSever   *
//  **************************

ESP8266WebServer server(80);

// hold uploaded file
fs::File fsUploadFile;



//  **************************
//  *   Interne Variablen    *
//  **************************

uint8_t brightness = 90;
int rounds = 0;
int dauer = 0;
int x    = matrix->width();
int pass = 0;
int pass1 = 0;
int color = 0;
int r = 255;
int g = 255;
int b = 255;
int r1 = 255;
int g1 = 255;
int b1 = 255;
long durchgang = 0;
int matrixBitmap[900];
char valueget[8];
String textts = "";
static char buf[15];                                    // je nach Format von "strftime" eventuell anpassen
bool clockSleep = 0;



//  **************************
//  *      Wetter API        *
//  **************************
unsigned long lastConnectionTime = 1;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 300000L; // Alle 5 Minuten neue Daten, um 50 Abfragen pro Tag nicht zu überschreiten
String partofday;
int regenmenge;
int weatherId;
float weatherTemperature;
int weatherHumidity;


WiFiClient client;


//  **************************
//  *    Intitialisieren     *
//  **************************

void setup() {

  Serial.begin(115200);

  // Logging all kinds of stuff :D

  Serial.println();
  Serial.print(F("Heap: ")); Serial.println(system_get_free_heap_size());
  Serial.print(F("Boot Vers: ")); Serial.println(system_get_boot_version());
  Serial.print(F("CPU: ")); Serial.println(system_get_cpu_freq());
  Serial.print(F("SDK: ")); Serial.println(system_get_sdk_version());
  Serial.print(F("Chip ID: ")); Serial.println(system_get_chip_id());
  Serial.print(F("Flash ID: ")); Serial.println(spi_flash_get_id());
  Serial.print(F("Flash Size: ")); Serial.println(ESP.getFlashChipRealSize());
  Serial.print(F("Vcc: ")); Serial.println(ESP.getVcc());
  Serial.println();



  //Load Settings from EEPROM
  EEPROM.begin(512);


  if (EEPROM.read(69) != 1) {

    //Write Default Settings

    EEPROM.write(0, brightness);
    EEPROM.write(1, UhrAnimation);
    EEPROM.write(2, FaceAnimation);
    EEPROM.write(3, Face2Animation);
    EEPROM.write(4, Face3Animation);
    EEPROM.write(5, FiresAnimation);
    EEPROM.write(6, DrinkAnimation);
    EEPROM.write(7, DigiDugAnimation);
    EEPROM.write(8, QbertAnimation);
    EEPROM.write(9, DemoReel1DugAnimation / 10);
    EEPROM.write(10, DemoReel1DugAnimation / 10);
    EEPROM.write(11, 20);
    EEPROM.write(59, 0); //Zeitschaltung1 deaktiviert
    EEPROM.write(60, 18); //Stunden (von)
    EEPROM.write(61, 0); //Minuten (von)
    EEPROM.write(62, 0); //Stunden (bis)
    EEPROM.write(63, 0); //Minuten (bis)
    EEPROM.write(64, 0); //Zeitschaltung2 deaktiviert
    EEPROM.write(65, 18); //Stunden (von)
    EEPROM.write(66, 0); //Minuten (von)
    EEPROM.write(67, 0); //Stunden (bis)
    EEPROM.write(68, 0); //Minuten (bis)
    EEPROM.write(69, 1);
    EEPROM.write(70, 1); // Autoplay
    EEPROM.write(71, 0);
    EEPROM.write(72, 0);
    EEPROM.write(73, 0);
    EEPROM.write(74, 0);
    EEPROM.write(75, 0);
    EEPROM.write(76, 0);
    EEPROM.write(77, 0);
    EEPROM.write(78, 0);  //Schriftart
    EEPROM.write(80, 0);  //R-Wert Uhr Hintergrund
    EEPROM.write(81, 0);  //G-Wert Uhr Hintergrund
    EEPROM.write(82, 0);  //B-Wert Uhr Hintergrund
    EEPROM.write(83, 0);  //Watchface
    EEPROM.write(84, 1);  //Legacy or Gif-Autoplay?
    EEPROM.write(85, 0);  //Wetter-Anzeige im Autoplay aktiviert?
    EEPROM.commit();
  } else {

    brightness = EEPROM.read(0);
    UhrAnimation = EEPROM.read(1);
    FaceAnimation = EEPROM.read(2);
    Face2Animation = EEPROM.read(3);
    Face3Animation = EEPROM.read(4);
    FiresAnimation = EEPROM.read(5);
    DrinkAnimation = EEPROM.read(6);
    DigiDugAnimation = EEPROM.read(7);
    QbertAnimation = EEPROM.read(8);
    DemoReel1DugAnimation = EEPROM.read(9) * 10;
    DemoReel2Animation = EEPROM.read(10) * 10;
    WeatherAnimation = EEPROM.read(11);

  }

  SPIFFS.begin();                           // Start the SPI Flash Files System


  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MILLI_AMPS);

  EEPROM.write(70, 1);              // Set Autoplay at boot up, because Gif blocks the Webserver (needs a fix)

  //  ***************************
  //  *       Starte WLAN       *
  //  ***************************

  //WiFiManager
  WiFiManager wifiManager;
  WiFi.hostname(host);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  wifiManager.setBreakAfterConfig(true);
  if (!wifiManager.autoConnect(host)) {
    Serial.println("failed to connect, we should reset and see if it connects");
    delay(5000);
    ESP.reset();
  }
  delay(3000);
  Serial.println("connected...yeey :)");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  delay(5000);



  //  ***************************
  //  *    Starte WEBSERVER     *
  //  ***************************

  if (WiFi.status() == WL_CONNECTED) {
    MDNS.begin(host);
    Serial.println(WiFi.hostname());
    server.serveStatic("/", SPIFFS, "/index.html");

    server.onNotFound(handleWebRequests);

    server.on("/listgifs", HTTP_GET, handleGifList);
    server.on("/deletegifs", HTTP_GET, handleGifDelete);
    server.on("/uploadgif", HTTP_POST, []() {
      server.send(200, "text/plain", "{\"success\":1}");
    }, handleGifUpload);


    server.on("/brightness", HTTP_GET, []() {
      String value = server.arg("value");
      setBrightness(value.toInt());
      sendInt(brightness);
    });

    server.on("/settingread", HTTP_GET, []() {
      int name = server.arg("name").toInt();
      int value = EEPROM.read(name);
      server.send(200, "text/json", String(value));
    });

    server.on("/settingwrite", HTTP_GET, []() {
      int name = server.arg("name").toInt();
      int value = server.arg("value").toInt();
      EEPROM.write(name, value);
      EEPROM.commit();
      server.send(200, "text/json", String(value));
    });


    server.on("/configwrite", HTTP_GET, []() {
      const size_t capacity = JSON_OBJECT_SIZE(2);
      DynamicJsonDocument doc(capacity);


      doc["weatherAPI"] = weatherAPI;
      doc["weatherLocation"] = weatherLocation;

      if (server.arg("name") == "weatherAPI") {
        doc["weatherAPI"] = server.arg("value");
      }

      if (server.arg("name") == "weatherLocation") {
        doc["weatherLocation"] = server.arg("value");
      }

      serializeJson(doc, Serial);

      File file = SPIFFS.open("/config.json", "w");
      file.print(Serial);
      file.close();

      server.send(200, "text/json", "ok");
    });


    server.on("/writesolidcolor", HTTP_GET, []() {
      String value = server.arg("value");
      long number = strtol(&value[0], NULL, 16);
      long red = number >> 16;
      long green = number >> 8 & 0xFF;
      long blue = number & 0xFF;
      EEPROM.write(72, red);
      EEPROM.write(73, green);
      EEPROM.write(74, blue);
      EEPROM.commit();
      server.send(200, "text/json", "ok");
    });

    server.on("/matrixpixel", HTTP_POST, []() {

      const size_t capacity = JSON_ARRAY_SIZE(900);
      DynamicJsonDocument doc(capacity);

      DeserializationError err = deserializeJson(doc, server.arg("data"));
      Serial.print(server.arg("data"));

      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
      }

      if (!copyArray(doc, matrixBitmap)) {                              /* CopyArray funktioniert öfters nicht.. */
        Serial.print("Error in copyArray occured");
        server.send(200, "text/json", "error occured" );
      }
      doc.clear();

      server.send(200, "text/json", "ok" );

    });

    server.on("/writetextcolor", HTTP_GET, []() {
      String value = server.arg("value");
      long number = strtol(&value[0], NULL, 16);
      long red = number >> 16;
      long green = number >> 8 & 0xFF;
      long blue = number & 0xFF;
      EEPROM.write(75, red);
      EEPROM.write(76, green);
      EEPROM.write(77, blue);
      EEPROM.commit();
      server.send(200, "text/json", "ok");
    });

    server.on("/restart", HTTP_GET, []() {
      server.send(200, "text/html", "Restart_Ok");
      FastLED.setBrightness(brightness);
        matrix->fillScreen(matrix->Color(128, 255, 0));
        matrix->setTextColor(matrix->Color(0, 0, 255));
        matrix->setCursor(1, 6);
        matrix->setFont(&Picopixel);
        matrix->print("Rload");
        matrix->setCursor(1, 13);
        matrix->print("Chaos");
        matrix->show();
      delay(500);
      ESP.restart();
    });

    server.on("/resetWifiConfig", HTTP_GET, []() {
        FastLED.setBrightness(brightness);
        matrix->fillScreen(matrix->Color(179, 30, 0));
        matrix->setTextColor(matrix->Color(0, 149, 179));
        matrix->setCursor(1, 6);
        matrix->setFont(&Picopixel);
        matrix->print("Reset");
        matrix->setCursor(1, 13);
        matrix->print("WiFi");
        matrix->show();
        
      server.send(200, "text/html", "ResetWifiConfig_ok");
        delay(500);
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
    });

    server.on("/scrolltext", HTTP_GET, []() {
      textts = server.arg("value");
      server.send(200, "text/html", "ok");
    });

    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {

        FastLED.setBrightness(brightness);
        matrix->fillScreen(matrix->Color(84, 92, 99));
        matrix->setTextColor(matrix->Color(124, 252, 0));
        matrix->setCursor(1, 9);
        matrix->setFont(&spacerock_minecraft4pt7b);
        matrix->print("Upd...");
        matrix->show();

        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Failed");
  }

  //  ***************************
  //  *      Starte Matrix      *
  //  ***************************

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(brightness);
  matrix->setFont(&TomThumb);


  //  ***************************
  //  *       Startscreen       *
  //  ***************************


  matrix->fillScreen(0);
  matrix->setTextColor(matrix->Color(255, 79, 0));
  matrix->setCursor(2, 6);
  matrix->print("LOAD");
  matrix->setCursor(0, 13);
  matrix->print("CHAOS");
  matrix->show();
  delay(1000);
  matrix->fillScreen(0);



  //  ***************************
  //  *      Setup Uhrzeit      *
  //  ***************************

  setupTime();

  updateWeather();

}

void handleGifDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS!");
  String path = server.arg(0);
  if (path == "/") return server.send(500, "text/plain", "BAD PATH!");
  if (!SPIFFS.exists(path)) return server.send(404, "text/plain", "FILE NOT FOUND!");
  SPIFFS.remove(path);
  Serial.println("DELETE: " + path);
  String msg = "deleted file: " + path;
  server.send(200, "text/plain", msg);
}

void handleGifUpload()
{
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    filename = "/gif" + filename;
    fsUploadFile = SPIFFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleGifList()
{
  String path = "/gif/";
  // Assuming there are no subdirectories
  fs::Dir dir = SPIFFS.openDir(path);
  String output = "";
  while (dir.next())
  {
    fs::File entry = dir.openFile("r");
    // Separate by comma if there are multiple files
    if (output != "")
      output += ",";
    output += String(entry.name()).substring(1);
    entry.close();
  }

  server.send(200, "text/plain", output);
}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);

}


void setupTime() {
  configTime(0, 0, ntpServer[1]);  // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);   // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
}

void updateWeather() {
  if (client.connect("api.weatherbit.io", 80))
  {
    Serial.println("connecting to weatherbit.io...");

    String url = "/v2.0/current?city=" + String(weatherLocation) + "&key=" + String(weatherAPI);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + "api.weatherbit.io" + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");


    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }


    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      return;
    }

    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(36) + 450;
    DynamicJsonDocument wea(capacity);
    DeserializationError error = deserializeJson(wea, client);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    JsonObject data_0 = wea["data"][0];
    JsonObject data_0_weather = data_0["weather"];
    weatherId = data_0_weather["code"];
    weatherTemperature = data_0["temp"];
    weatherHumidity = data_0["rh"];
    partofday = data_0["pod"].as<String>();
    regenmenge = data_0["precip"];

    Serial.println(weatherId);
    lastConnectionTime = millis();

  } else {
    Serial.println("Error connecting to WeatherAPI");
    return;
  }
}

void setBrightness(uint8_t value)
{
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;

  brightness = value;

  FastLED.setBrightness(brightness);

  EEPROM.write(0, brightness);
  EEPROM.commit();

  broadcastInt("brightness", brightness);
}

void broadcastInt(String name, uint8_t value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
  //  webSocketsServer.broadcastTXT(json);
}

void broadcastString(String name, String value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
  //  webSocketsServer.broadcastTXT(json);
}

void showBitmap()
{
  FastLED.setBrightness(brightness);
  matrix->clear();
  int x_pos = 0;
  int y_pos = -1;

  for (int i = 0; i < 300; i++) {

    x_pos = i % 20;
    if (i % 20 == 0) {
      y_pos++;
    }

    matrix->drawPixel(x_pos, y_pos, {matrixBitmap[i], matrixBitmap[i + 300], matrixBitmap[i + 600]});


  }
  matrix->show();
  FastLED.show();
  delay(500);

}

void sendInt(uint8_t value)
{
  sendString(String(value));
}

void sendString(String value)
{
  server.send(200, "text/plain", value);
}

//  ***************************
//  *  Animationsdefinitonen  *
//  ***************************

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {Uhrzeit, Face, Face2, Face3, Fires, Drink, DigiDug, Qbert, rainbow, confetti, rainbowWithGlitter, sinelon, bpm, juggle};

String Pattern;
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


// ############################## LOOOP ################################################################################################################################################





//  ***************************
//  *  Animationen ausführen  *
//  ***************************


void loop() {


  server.handleClient();
  MDNS.update();

  if (!checkClockSleep()) {


    //  *********************************************************************************************************
    //  * Wenn EEPROM-Variable = 1, dann zeige Animationen, ansonsten zeige nur gewünschte Animation dauerhaft. *
    //  *********************************************************************************************************

    if (EEPROM.read(70) == 1) {

      if (EEPROM.read(84) == 0) {         //Legacy-Animationen

        if (gCurrentPatternNumber == 0) {

          Uhrzeit();
          if (EEPROM.read(83) == 0) {
            if (durchgang >= UhrAnimation) {
              durchgang = 0;
              nextPattern();
            }
          } else {
            if (durchgang >= UhrAnimation * 80) {
              durchgang = 0;
              nextPattern();
            }
          }
        }

        else {
          Serial.print("Pattern: ");
          Serial.println(gCurrentPatternNumber);
          Serial.println();
          gPatterns[gCurrentPatternNumber]();
          gHue++;
          if (gCurrentPatternNumber == 8 || gCurrentPatternNumber == 9 || gCurrentPatternNumber == 10 || gCurrentPatternNumber == 11) {
            if (rounds >= DemoReel1DugAnimation) {
              rounds = 0;
              FastLED.clear();
              nextPattern();
            }
          }
          else if (gCurrentPatternNumber == 12 || gCurrentPatternNumber == 13) {
            if (rounds >= DemoReel2Animation) {
              rounds = 0;
              FastLED.clear();
              nextPattern();
            }
          }
        }
      } else {   //GIF - ANIMATIONEN ////////////////////////////////////


        if (EEPROM.read(85) == 1) { // WETTER AKTIVIERT


          if (EEPROM.read(83) == 0) {
            if (durchgang >= UhrAnimation + EEPROM.read(11)) {
              durchgang = 0;
              showGif();
            } else {
              if (durchgang >= UhrAnimation) {
                showWeather();
              } else {
                Uhrzeit();
              }
            }
          } else {
            if (durchgang >= UhrAnimation * 50 + EEPROM.read(11)) {
              durchgang = 0;
              showGif();
            } else {
              if (durchgang >= UhrAnimation * 50) {
                showWeather();
              } else {
                Uhrzeit();
              }
            }
          }





        } else {    // WETTER DEAKTIVIERT



          if (EEPROM.read(83) == 0) {
            if (durchgang >= UhrAnimation) {
              durchgang = 0;
              showGif();
            } else {
              Uhrzeit();
            }
          } else {
            if (durchgang >= UhrAnimation * 50) {
              durchgang = 0;
              showGif();
            } else {
              Uhrzeit();
            }
          }

        }



      }
    } else {

      //  ******************************
      //  *     Standbild-Animation    *
      //  ******************************

      if (EEPROM.read(71) == 0) {
        Uhrzeit();
      }
      if (EEPROM.read(71) == 1) {
        showText();
      }
      if (EEPROM.read(71) == 2) {
        solidColor();
      }
      if (EEPROM.read(71) == 16) {
        showBitmap();
      };
      if (EEPROM.read(71) == 17) {
        showWeather();
      };
      if (EEPROM.read(71) == 18) {
        showGif();
      };
      if (EEPROM.read(71) == 3) {
        Face();
      }
      if (EEPROM.read(71) == 4) {
        Face2();
      }
      if (EEPROM.read(71) == 5) {
        Face3();
      }
      if (EEPROM.read(71) == 6) {
        Fires();
      }
      if (EEPROM.read(71) == 7) {
        Drink();
      }
      if (EEPROM.read(71) == 8) {
        DigiDug();
      }
      if (EEPROM.read(71) == 9) {
        Qbert();
      }
      if (EEPROM.read(71) == 10) {
        rainbow();
      }
      if (EEPROM.read(71) == 11) {
        rainbowWithGlitter();
      }
      if (EEPROM.read(71) == 12) {
        confetti();
      }
      if (EEPROM.read(71) == 13) {
        sinelon();
      }
      if (EEPROM.read(71) == 14) {
        bpm();
      }
      if (EEPROM.read(71) == 15) {
        juggle();
      }

    }
  } else {
    matrix->fillScreen(0);
    matrix->show();
    delay(0);
  }

}

//  ***************************
//  *     Uhrzeit anzeigen    *
//  ***************************

void Uhrzeit() {

  FastLED.setBrightness(brightness);
  static time_t lastsek {0};
  time_t now = time(&now);
  localtime_r(&now, &lt);
  if (lt.tm_sec != lastsek) {
    lastsek = lt.tm_sec;
    if (!(time(&now))) {                         // einmal am Tag die Zeit vom NTP Server holen o. jede Stunde "% 3600" aller zwei "% 7200"
      setupTime();
    }
    strftime (buf, sizeof(buf), "%d.%m.%Y", &lt);      // http://www.cplusplus.com/reference/ctime/strftime/
  }

  matrix->clear();

  // Uhr-Hintergrund anzeigen

  matrix->fillScreen(matrix->Color(EEPROM.read(80), EEPROM.read(81), EEPROM.read(82)));




  if (EEPROM.read(78) == 0) {
    matrix->setFont(&TomThumb);
  }
  if (EEPROM.read(78) == 1) {
    matrix->setFont(&Picopixel);
  }
  if (EEPROM.read(78) == 2) {
    matrix->setFont(&apridot4pt7b);
  }
  if (EEPROM.read(78) == 3) {
    matrix->setFont(&spacerock_minecraft4pt7b);
  }
  if (EEPROM.read(78) == 4) {
    matrix->setFont(&art_bitco4pt7b);
  }

  matrix->setTextColor(matrix->Color(r1, g1, b1));


  //Uhrzeit Standard Watchface

  if (EEPROM.read(83) == 0) {

    if (EEPROM.read(78) == 0 or EEPROM.read(78) == 1 or EEPROM.read(78 == 5)) {
      matrix->setCursor(2, 6);
    }

    if (EEPROM.read(78) == 2 or EEPROM.read(78) == 3 or EEPROM.read(78) == 4) {
      matrix->setCursor(0, 6);
    }

    if (EEPROM.read(78) == 2 or EEPROM.read(78) == 3 or EEPROM.read(78) == 4) {
      matrix->printf("%.2d%.2d", lt.tm_hour, lt.tm_min);
    } else {
      matrix->printf("%.2d:%.2d", lt.tm_hour, lt.tm_min);
    }

  }

  // SimpleClock Watchface

  if (EEPROM.read(83) == 2) {

    matrix->setFont(&spacerock_minecraft4pt7b);
    matrix->fillScreen(0);
    matrix->setCursor(3, 6);
    matrix->setTextColor(matrix->Color(31, 102, 229));
    matrix->printf("%.2d", lt.tm_hour);
    matrix->setCursor(7, 13);
    matrix->setTextColor(matrix->Color(31, 201, 229));
    matrix->printf("%.2d", lt.tm_min);

  }



  // Game Frame Clone Watchface

  if (EEPROM.read(83) == 1) {
    int pixelsecx;
    int pixelsecy;

    matrix->setFont(&to_infinity3pt7b);
    matrix->fillScreen(0);
    //matrix->setCursor(1, 9);
    matrix->setCursor(0, 11);
    matrix->setTextColor(matrix->Color(255, 20, 147));
    matrix->printf("%.2d", lt.tm_hour);
    //matrix->setCursor(10, 9);
    matrix->setCursor(10, 11);
    matrix->setTextColor(matrix->Color(20, 255, 246));
    matrix->printf("%.2d", lt.tm_min);
    if (lt.tm_sec < 20) {
      pixelsecy = 0;
    } else {
      if (lt.tm_sec >= 40) {
        //pixelsecy = 10;
        pixelsecy = 3;
      } else {
        pixelsecy = 14;
      }
    }
    pixelsecx = lt.tm_sec % 20;
    matrix->drawPixel(pixelsecx, pixelsecy, CRGB(255, 255, 255));

  }


  //Wenn Standard-Watchface ausgewählt, Zeige Datum mit Laufschrift.
  if (EEPROM.read(83) == 0) {
    Datum();

  } else {

    matrix->show();
    delay(120);
    durchgang++;

  }
}

//  ***************************
//  *      Scroll Datum       *
//  ***************************

void Datum() {
  char datum[30];
  sprintf(datum, "%s   %s", dayNames[lt.tm_wday], buf);
  int scroll = (strlen(datum) * -3) - ((strlen(dayNames[lt.tm_wday])) * 2) + 3 ; //Anzahl Pixel + Leerzeichen
  matrix->setTextColor(matrix->Color(r, g, b));
  matrix->setCursor(x, 13);
  if (EEPROM.read(78) == 0) {
    matrix->setFont(&TomThumb);
  }
  if (EEPROM.read(78) == 1) {
    matrix->setFont(&Picopixel);
  }
  if (EEPROM.read(78) == 2) {
    matrix->setFont(&apridot4pt7b);
  }
  if (EEPROM.read(78) == 3) {
    matrix->setFont(&spacerock_minecraft4pt7b);
  }
  if (EEPROM.read(78) == 4) {
    matrix->setFont(&art_bitco4pt7b);
  }

  matrix->printf(datum);
  if (--x < scroll) {
    x = matrix->width();
    DatumFarbWechsel();
    durchgang++;
  }
  matrix->show();
  delay(125);
}

void showText() {
  FastLED.setBrightness(brightness);
  matrix->clear();
  int scroll = textts.length() * -3 - 10;
  matrix->setTextColor(matrix->Color(EEPROM.read(75), EEPROM.read(76), EEPROM.read(77)));
  matrix-> setCursor(x, 9);
  if (EEPROM.read(78) == 0) {
    matrix->setFont(&TomThumb);
  }
  if (EEPROM.read(78) == 1) {
    matrix->setFont(&Picopixel);
  }
  if (EEPROM.read(78) == 2) {
    matrix->setFont(&spacerock_minecraft4pt7b);
  }
  if (EEPROM.read(78) == 4) {
    matrix->setFont(&art_bitco4pt7b);
  }

  matrix->print(textts);
  if (--x < scroll) {
    x = matrix->width();
    DatumFarbWechsel();
    durchgang++;
  }
  matrix->show();
  delay(125);

}

void solidColor() {
  FastLED.setBrightness(brightness);
  matrix->clear();
  fill_solid( leds, NUM_LEDS, CRGB(EEPROM.read(72), EEPROM.read(73), EEPROM.read(74)));
  matrix->show();
}


void showWeather() {


  if (millis() - lastConnectionTime > postingInterval) {
    updateWeather();
  }


  int wtemprounded = (int)(weatherTemperature + .5);
  FastLED.setBrightness(brightness);
  matrix->clear();
  matrix->setTextColor(matrix->Color(255, 255, 255));
  matrix->setCursor(6, 14);
  matrix->setFont(&TomThumb);
  matrix->print(wtemprounded);
  matrix->print("C");
  matrix->drawPixel(18, 9, CRGB(255, 255, 255));

  if (weatherId == 801 || weatherId == 802) {

    if (partofday == "d") {
      File imageFile = SPIFFS.open("/weather/cloudy.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    } else {
      File imageFile = SPIFFS.open("/weather/cloudy_n.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    }


  }


  if (weatherId == 803 || weatherId == 804) {

    File imageFile = SPIFFS.open("/weather/cloudynosun.gif", "r");
    gifPlayer.setFile(imageFile);
    gifPlayer.parseGifHeader();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    gifPlayer.drawFrame();

  }


  if (weatherId == 800) {
    if (partofday == "d") {
      File imageFile = SPIFFS.open("/weather/sunny.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    } else {
      File imageFile = SPIFFS.open("/weather/sunny_n.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    }


  }


  if (weatherId == 600 || weatherId == 601 || weatherId == 602 || weatherId == 610 || weatherId == 621 || weatherId == 622 || weatherId == 623 || weatherId == 611 || weatherId == 612) {

    File imageFile = SPIFFS.open("/weather/snow.gif", "r");
    gifPlayer.setFile(imageFile);
    gifPlayer.parseGifHeader();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    gifPlayer.drawFrame();

  }


  if (weatherId == 300 || weatherId == 301 || weatherId == 302 || weatherId == 500 || weatherId == 501 || weatherId == 502 || weatherId == 511 || weatherId == 520 || weatherId == 900) {

    File imageFile = SPIFFS.open("/weather/rainy.gif", "r");
    gifPlayer.setFile(imageFile);
    gifPlayer.parseGifHeader();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    gifPlayer.drawFrame();

  }



  if (weatherId == 200 || weatherId == 201 || weatherId == 202 || weatherId == 230 || weatherId == 231 || weatherId == 232 || weatherId == 233) {

    File imageFile = SPIFFS.open("/weather/thunder.gif", "r");
    gifPlayer.setFile(imageFile);
    gifPlayer.parseGifHeader();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    gifPlayer.drawFrame();

  }


  if (weatherId == 522) {

    File imageFile = SPIFFS.open("/weather/heavyrain.gif", "r");
    gifPlayer.setFile(imageFile);
    gifPlayer.parseGifHeader();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    gifPlayer.drawFrame();

  }


  if (weatherId == 521) {
    if (partofday == "d") {
      File imageFile = SPIFFS.open("/weather/rainsunny.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    } else {
      File imageFile = SPIFFS.open("/weather/rainsunny_n.gif", "r");
      gifPlayer.setFile(imageFile);
      gifPlayer.parseGifHeader();
      gifPlayer.parseLogicalScreenDescriptor();
      gifPlayer.parseGlobalColorTable();
      gifPlayer.drawFrame();
    }


  }

  matrix->show();
  delay(125);
  durchgang++;
}




void showGif() {

  // Todo: Implement a timer, to control length of Gif-Animation

  Serial.print("GifReader started");
  String fileN;
  int num = 0;
  Dir dir = SPIFFS.openDir("/gif");
  while (dir.next()) {
    num++;
  }

  int ran = random(num);
  Serial.println("Random Number");
  Serial.println(num);

  Dir folder = SPIFFS.openDir("/gif");

  for (int i = 0; i <= ran; i++) {


    Serial.println(i);
    folder.next();

    fileN = folder.fileName();

  }

  uint32_t result;

  Serial.print("Reading ");
  Serial.println(fileN);

  File imageFile = SPIFFS.open(fileN, "r");
  if (!imageFile) {
    Serial.println("Failed to open");
    return;
  }

  gifPlayer.setFile(imageFile);

  for (uint8_t c = 0; c < 10; c++) {
    if (!gifPlayer.parseGifHeader()) {
      imageFile.close();
      Serial.println("No gif header");
      return;
    }

    matrix->clear();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    Serial.println("Processing gif");
    do {                                            // Very bad code!! needs to be a non-blocking function.
      gifPlayer.drawFrame();
      result = gifPlayer.drawFrame();
      matrix->show();
      delay(0);
    } while (result != ERROR_FINISHED);
    imageFile.seek(0);
  }

  Serial.println("Gif finished");
  imageFile.close();

  durchgang++;

}

// CLOCKSLEEP #####################

bool checkClockSleep() {

  if (EEPROM.read(64) == 0 && EEPROM.read(59) == 0) {
    return 0;
  }

  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  long minutesinday = (timeinfo->tm_hour) * 60 + (timeinfo->tm_min);
  long t1_1 = EEPROM.read(60) * 60 + EEPROM.read(61);
  long t1_2 = EEPROM.read(62) * 60 + EEPROM.read(63);
  long t2_1 = EEPROM.read(64) * 60 + EEPROM.read(66);
  long t2_2 = EEPROM.read(67) * 60 + EEPROM.read(68);
  int check = 0;


  if (EEPROM.read(59) == 1) {
    if (t1_1 - t1_2 < 0) {
      if (t1_1 < minutesinday && t1_2 > minutesinday) {
        return 0;
      } else {
        if (EEPROM.read(64) == 0) {
          return 1;
        }
      }
    }
    else {
      if ((t1_1 < minutesinday && t1_2 < minutesinday) || (t1_1 > minutesinday && t1_2 > minutesinday)) {
        return 0;
      } else {
        if (EEPROM.read(64) == 0) {
          return 1;
        }
      }
    }
  }

  if (EEPROM.read(64) == 1) {
    if (t2_1 - t2_2 < 0) {
      if (t2_1 < minutesinday && t2_2 > minutesinday) {
        return 0;
      } else {
        if (EEPROM.read(59) == 0) {
          return 1;
        }
      }
    }
    else {
      if ((t2_1 < minutesinday && t2_2 < minutesinday) || (t2_1 > minutesinday && t2_2 > minutesinday)) {
        return 0;
      } else {
        if (EEPROM.read(59) == 0) {
          return 1;
        }
      }
    }
  }

  return 1;


}



//  ***************************
//  *    Nächste Animation    *
//  ***************************

void nextPattern() {
  int lastpattern = gCurrentPatternNumber;
  //int pattern;
  if (lastpattern == 0) {
    gCurrentPatternNumber = random(ARRAY_SIZE( gPatterns));
    while (lastpattern == gCurrentPatternNumber and EEPROM.read(gCurrentPatternNumber + 1) != 0) {
      gCurrentPatternNumber = random(ARRAY_SIZE( gPatterns));
    }
  }
  else {
    UhrzeitFarbWechsel();
    gCurrentPatternNumber = 0;
  }
}


//  ***************************
//  *     Textfarbe Datum     *
//  ***************************
void DatumFarbWechsel() {
  if (pass == 0) {
    //Datum
    r = random(100, 255);
    g = random(100, 255);
    b = 255;
    pass = 1;
  }
  else if (pass == 1) {
    //Datum
    r = random(100, 255);
    g = 255;
    b = random(100, 255);
    pass = 2;
  }
  else if (pass == 2) {
    //Datum
    r = 255;
    g = random(100, 255);
    b = random(100, 255);
    pass = 0;
  }
  else {
    pass = 0;
  }
}

//  ***************************
//  *    Textfarbe Uhrzeit    *
//  ***************************
void UhrzeitFarbWechsel() {
  if (pass1 == 0) {
    //Uhrzeit
    r1 = 255;
    g1 = 255;
    b1 = random(100, 255);
    pass1 = 1;
  }
  else if (pass1 == 1) {
    r1 = 255;
    g1 = random(100, 255);
    b1 = 255;
    pass1 = 2;
  }
  else if (pass1 == 2) {
    r1 = random(100, 255);
    g1 = 255;
    b1 = 255;
    pass1 = 0;
  }
  else {
    r1 = 255;
    g1 = 255;
    b1 = 255;
    pass1 = 0;
  }
}





//  ***************************
//  *    Figur Animationen    *
//  ***************************

void Drink() {
  int rounds = 0;
  while (rounds <= DrinkAnimation) {
    //    1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink01[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink02[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    3rd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink03[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    4th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink04[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    5th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink05[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    6th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink06[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    7th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink07[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    8th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink08[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    9th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink09[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    10th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink010[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    11th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink011[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    12th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink012[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    13th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink013[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    14th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink014[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    15th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink015[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);

    //    16th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Drink016[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(200);
    rounds++;
  }
  nextPattern();
}


void Qbert() {
  int rounds = 0;
  while (rounds <= QbertAnimation) {
    //    1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Qbert01[i]));  // Read array from Flash
    }
    FastLED.show();
    delay(500);


    //    2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Qbert02[i]));
    }
    FastLED.show();
    delay(500);
    rounds++;
  }
  nextPattern();
}

void DigiDug() {
  int rounds = 0;
  while (rounds <= DigiDugAnimation) {
    //    1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(DigDug01[i]));
    }
    FastLED.show();
    delay(250);

    //    2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(DigDug02[i]));
    }
    FastLED.show();
    delay(250);
    rounds++;
  }
  nextPattern();
}

void Face() {
  int rounds = 0;
  while (rounds < FaceAnimation) {
    //    1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face001[i]));
    }
    FastLED.show();
    delay(2000);

    //    2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face002[i]));
    }
    FastLED.show();
    delay(1000);

    //    3rd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face003[i]));
    }
    FastLED.show();
    delay(1000);

    //    4th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face001[i]));
    }
    FastLED.show();
    delay(1000);

    //    5th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS ; i++) {
      leds[i] = pgm_read_dword(&(Face004[i]));
    }
    FastLED.show();
    delay(40);

    //    6th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face005[i]));
    }
    FastLED.show();
    delay(40);

    //    7th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face007[i]));
    }
    FastLED.show();
    delay(40);

    //    8th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face008[i]));
    }
    FastLED.show();
    delay(60);

    //    9th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face007[i]));
    }
    FastLED.show();
    delay(40);

    //    10th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face005[i]));
    }
    FastLED.show();
    delay(40);

    //    11th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS + 1; i++) {
      leds[i] = pgm_read_dword(&(Face004[i]));
    }
    FastLED.show();
    delay(40);
    rounds++;
  }
  nextPattern();
}

void Face2() {
  int rounds = 0;
  while (rounds < Face2Animation) {
    //    1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face202[i]));
    }
    FastLED.show();
    delay(700);

    //    2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face203[i]));
    }
    FastLED.show();
    delay(700);
    rounds++;
  }
  nextPattern();
}

void Face3() {
  int rounds = 0;
  while (rounds < Face3Animation) {
    //  1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face301[i]));
    }
    FastLED.show();
    delay(100);

    //  2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face302[i]));
    }
    FastLED.show();
    delay(100);

    //  3rd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face303[i]));
    }
    FastLED.show();
    delay(100);

    //  4th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face304[i]));
    }
    FastLED.show();
    delay(100);

    //  5th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face305[i]));
    }
    FastLED.show();
    delay(100);

    //  6th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face306[i]));
    }
    FastLED.show();
    delay(100);

    //  7th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face307[i]));
    }
    FastLED.show();
    delay(100);

    //  8th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face308[i]));
    }
    FastLED.show();
    delay(100);

    //  9th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face309[i]));
    }
    FastLED.show();
    delay(100);

    //  10th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face310[i]));
    }
    FastLED.show();
    delay(100);

    //  11th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face311[i]));
    }
    FastLED.show();
    delay(100);

    //  12th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face312[i]));
    }
    FastLED.show();
    delay(100);

    //  13th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face313[i]));
    }
    FastLED.show();
    delay(100);

    //  14th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face314[i]));
    }
    FastLED.show();
    delay(100);

    //  15th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Face315[i]));
    }
    FastLED.show();
    delay(100);
    rounds++;
  }
  nextPattern();
}

void Fires() {
  int rounds = 0;
  while (rounds < FiresAnimation) {
    //  1st Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer1[i]));
    }
    FastLED.show();
    delay(80);

    //  2nd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer2[i]));
    }
    FastLED.show();
    delay(80);

    //  3rd Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer3[i]));
    }
    FastLED.show();
    delay(80);

    //  4th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer4[i]));
    }
    FastLED.show();
    delay(80);

    //  5th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer5[i]));
    }
    FastLED.show();
    delay(80);

    //  6th Frame
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = pgm_read_dword(&(Feuer6[i]));
    }
    FastLED.show();
    delay(80);
    rounds++;
  }
  nextPattern();
}

//  ***************************
//  *   DEMO Reel Animation   *
//  ***************************

void rainbow() {
  FastLED.setBrightness(brightness);
  fill_rainbow( leds, NUM_LEDS, gHue * 2, 7);
  FastLED.show();
  rounds++;
}

void rainbowWithGlitter() {
  FastLED.setBrightness(brightness);
  fill_rainbow( leds, NUM_LEDS, gHue * 2, 7);
  addGlitter(50);
  FastLED.show();
  rounds++;
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  FastLED.setBrightness(brightness * 2);
  fadeToBlackBy( leds, NUM_LEDS, 50);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue * 2 + random16(150), 210, 255);
  FastLED.show();
  rounds++;
}

void sinelon() {
  FastLED.setBrightness(brightness * 2);
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = beatsin16( 25, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 210);
  FastLED.show();
  rounds++;
}

void bpm() {
  FastLED.setBrightness(brightness);
  uint8_t BeatsPerMinute = 45;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  FastLED.show();
  FastLED.delay(10);
  rounds++;

}

void juggle() {
  FastLED.setBrightness(brightness);
  fadeToBlackBy( leds, NUM_LEDS, 60);
  byte dothue = 0;
  for ( int i = 0; i < 9; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 210, 255);
    dothue += 28;
  }
  FastLED.show();
  FastLED.delay(10);
  rounds++;

}
