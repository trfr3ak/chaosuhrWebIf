

// ******************************************************************************
// *************            ChaosUhr mit Webinterface.              *************
// *************       Sketch basiert auf dem von Micha1986         *************
// *************       erweitert und ergänzt von Jonas Krug         *************
// ******************************************************************************

// Version 0.12

#include <FS.h>

#define FASTLED_FORCE_SOFTWARE_SPI

// ----------------------------
// Standard Libraries
// ----------------------------
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



// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <ArduinoJson.h>

#include <ArduinoSpotify.h>
// Library for connecting to the Spotify API

// Install from Github
// https://github.com/witnessmenow/arduino-spotify-api



#include "GifPlayer.h"
GifPlayer gifPlayer;      //Takes up a looooot of Ram!
#include "Config.h"


// Fonts
#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>
#include "Fonts/apridot4pt7b.h"
#include "Fonts/spacerock_minecraft4pt7b.h"
#include "Fonts/art_bitco4pt7b.h"
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


//  **************************
//  *   Definiere WebSever   *
//  **************************

ESP8266WebServer server(80);

// hold uploaded file
fs::File fsUploadFile;





//  **************************
//  *   Interne Variablen    *
//  **************************

int rounds = 0;
int x = matrix->width();
uint8_t pass = 0;
uint8_t pass1 = 0;

uint8_t r = 255;
uint8_t g = 255;
uint8_t b = 255;
uint8_t r1 = 255;
uint8_t g1 = 255;
uint8_t b1 = 255;
int durchgang = 0;
uint8_t matrixBitmap[900];
String textts = "";                                     //Todo: Save tts to JSON file, to save this global Variable
static char buf[15];                                    // je nach Format von "strftime" eventuell anpassen
bool clockSleep = 0;


WiFiClientSecure client;
WiFiClient client80;

//  **************************
//  *      Spotify           *
//  **************************

ArduinoSpotify spotify(client, SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, SPOTIFY_REFRESH_TOKEN);
unsigned long delayBetweenRequests = 60000; // Time between requests (1 minute)
unsigned long requestDueTime;               //time when request due
String spotifyTrack;
String spotifyArtist;
bool spotifyPlaying;

// Hier noch host einfügen!!
char callbackURI[] = "http%3A%2F%2Fchaosuhr.local%2Fcallback%2F";




//  **************************
//  *      Wetter API        *
//  **************************
unsigned long lastConnectionTime = 1;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 300000L; // Alle 5 Minuten neue Daten, um 50 Abfragen pro Tag nicht zu überschreiten
bool partofday;
int regenmenge;
int weatherId;
int weatherTemperature;
int weatherHumidity;





//  **************************
//  *    Intitialisieren     *
//  **************************

void setup() {

  Serial.begin(115200);

  //Load Settings from EEPROM
  EEPROM.begin(128);


  if (EEPROM.read(69) != 1) {

    //Write Default Settings

    EEPROM.write(0, 90);    // brightness
    EEPROM.write(1, 2);     // UhrAnimation
    EEPROM.write(2, 4); // FaceAnimation
    EEPROM.write(3, 10);  // Face2Animation
    EEPROM.write(4, 4);  // Face3Animation
    EEPROM.write(5, 15);  // FiresAnimation
    EEPROM.write(6, 3);  // DrinkAnimation
    EEPROM.write(7, 8);  // DigiDugAnimation
    EEPROM.write(8, 5);  // QbertAnimation
    EEPROM.write(9, 1100 / 10);    //DemoReel1DugAnimation
    EEPROM.write(10, 510 / 10);   //DemoReel1DugAnimation
    EEPROM.write(11, 20);  // WetterAnimation
    EEPROM.write(59, 0); // Zeitschaltung1 deaktiviert
    EEPROM.write(60, 18); // Stunden (von)
    EEPROM.write(61, 0); // Minuten (von)
    EEPROM.write(62, 0); // Stunden (bis)
    EEPROM.write(63, 0); // Minuten (bis)
    EEPROM.write(64, 0); // Zeitschaltung2 deaktiviert
    EEPROM.write(65, 18); // Stunden (von)
    EEPROM.write(66, 0); // Minuten (von)
    EEPROM.write(67, 0); // Stunden (bis)
    EEPROM.write(68, 0); // Minuten (bis)
    EEPROM.write(69, 1); // Ersteinrichtung
    EEPROM.write(70, 1); // Autoplay
    EEPROM.write(71, 0); // Standbild-Animation
    EEPROM.write(72, 0);
    EEPROM.write(73, 0);
    EEPROM.write(74, 0);
    EEPROM.write(75, 0);
    EEPROM.write(76, 0);
    EEPROM.write(77, 0);
    EEPROM.write(78, 0);  // Schriftart
    EEPROM.write(80, 0);  // R-Wert Uhr Hintergrund
    EEPROM.write(81, 0);  // G-Wert Uhr Hintergrund
    EEPROM.write(82, 0);  // B-Wert Uhr Hintergrund
    EEPROM.write(83, 0);  // Watchface
    EEPROM.write(84, 1);  // Legacy or Gif-Autoplay?
    EEPROM.write(85, 0);  // Wetter-Anzeige im Autoplay aktiviert?
    EEPROM.write(100, 10); // Anzahl Wiederholungen der Gif-Animation
    EEPROM.commit();
  }

  SPIFFS.begin();                           // Start the SPI Flash Files System


  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(EEPROM.read(0));
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MILLI_AMPS);

  if (EEPROM.read(71) == 18) EEPROM.write(70, 1);              // Set Autoplay at boot up, because Gif blocks the Webserver (needs a fix)

  //  ***************************
  //  *       Starte WLAN       *
  //  ***************************

  WiFiManager wifiManager;
  WiFi.hostname(HOSTNAME);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  wifiManager.setBreakAfterConfig(true);
  if (!wifiManager.autoConnect(HOSTNAME)) {

    delay(5000);
    ESP.reset();
  }
  delay(3000);

  delay(5000);



  //  ***************************
  //  *    Starte WEBSERVER     *
  //  ***************************

  if (WiFi.status() == WL_CONNECTED) {
    MDNS.begin(HOSTNAME);
    Serial.println(WiFi.hostname());
    server.serveStatic("/", SPIFFS, "/index.html");

    server.onNotFound(handleWebRequests);

    server.on("/listgifs", HTTP_GET, handleGifList);
    server.on("/deletegifs", HTTP_GET, handleGifDelete);
    server.on("/uploadgif", HTTP_POST, []() {
      server.send(200, "text/plain", "{\"success\":1}");
    }, handleGifUpload);


    server.on("/brightness", HTTP_GET, []() {
      setBrightness(server.arg("value").toInt());
    });

    server.on("/getSPIFFSsize", HTTP_GET, []() {
      FSInfo fs_info;
      if (!SPIFFS.info(fs_info)) {
        server.send(400, "text/json", "error");
      }
      server.send(200, "text/json", fs_info.totalBytes + "#" + fs_info.usedBytes);
    });

    server.on("/settingread", HTTP_GET, []() {

      server.send(200, "text/json", String(EEPROM.read(server.arg("name").toInt())));
    });

    server.on("/settingwrite", HTTP_GET, []() {
      int name = server.arg("name").toInt();
      int value = server.arg("value").toInt();
      EEPROM.write(name, value);
      EEPROM.commit();
      server.send(200, "text/json", String(value));
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
      StaticJsonDocument<capacity> mat;

      DeserializationError err = deserializeJson(mat, server.arg("data"));
      Serial.print(server.arg("data"));

      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
      }

      if (!copyArray(mat, matrixBitmap)) {                              /* CopyArray funktioniert öfters nicht.. */
        server.send(200, "text/json", "error occured" );
      }
      mat.clear();

      server.send(200, "text/json", "ok" );

    });

    server.on("/writetextcolor", HTTP_GET, []() {
      String value = server.arg("value");
      long number = strtol(&value[0], NULL, 16);
      uint8_t red = number >> 16;
      uint8_t green = number >> 8 & 0xFF;
      uint8_t blue = number & 0xFF;
      EEPROM.write(75, red);
      EEPROM.write(76, green);
      EEPROM.write(77, blue);
      EEPROM.commit();
      server.send(200, "text/json", "ok");
    });

    server.on("/restart", HTTP_GET, []() {
      server.send(200, "text/html", "Restart_Ok");
      FastLED.setBrightness(EEPROM.read(0));
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
      FastLED.setBrightness(EEPROM.read(0));
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

    server.on("/spotify", handleSpotifyRoot);
    server.on("/callback/", handleSpotifyCallback);

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

        FastLED.setBrightness(EEPROM.read(0));
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



  } else {
    Serial.println(F("WiFi Failed"));
  }



  //  ***************************
  //  *      Setup Uhrzeit      *
  //  ***************************


  setupTime();



  //  ***************************
  //  *      Setup Wetter       *
  //  ***************************


  updateWeather();

  Serial.println(ESP.getFreeHeap(), DEC);

  //  ***************************
  //  *      Setup Spotify      *
  //  ***************************



  // Crashes the ESP8266.. needs further investigation - Probably  Stack overflow -- Need to clean up Ram
/*
  client.setFingerprint(SPOTIFY_FINGERPRINT);

  Serial.println(F("Refreshing Access Tokens"));
  if (!spotify.refreshAccessToken()) {
    Serial.println(F("Failed to get access tokens"));
  }
*/





  //  ***************************
  //  *      Starte Matrix      *
  //  ***************************

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(EEPROM.read(0));
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
  delay(500);



}

void handleGifDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS!");
  String path = server.arg(0);
  if (path == "/") return server.send(500, "text/plain", "BAD PATH!");
  if (!SPIFFS.exists(path)) return server.send(404, "text/plain", "FILE NOT FOUND!");
  SPIFFS.remove(path);
  String msg = "deleted file:" + path;
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

}


void setupTime() {
  configTime(0, 0, ntpServer[1]);  // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);   // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
}

void updateWeather() {

  

  if (client80.connect("api.weatherbit.io", 80))
  {

    String url = "/v2.0/current?city=" + String(WEATHER_LOCATION) + "&key=" + String(WEATHER_API);

    client80.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + "api.weatherbit.io" + "\r\n" +
                   "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Connection: close\r\n\r\n");


    // Check HTTP status
    char status[32] = {0};
    client80.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }


    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client80.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      return;
    }

    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(36) + 450;
    DynamicJsonDocument wea(capacity);
    DeserializationError error = deserializeJson(wea, client80);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    JsonObject data_0 = wea["data"][0];
    JsonObject data_0_weather = data_0["weather"];
    weatherId = data_0_weather["code"];
    weatherTemperature = (int)(data_0["temp"].as<float>() + .5);
    weatherHumidity = data_0["rh"];
    if (data_0["pod"].as<String>() == "d") {
      partofday = 0;
    } else {
      partofday = 1;
    }
    regenmenge = data_0["precip"];

    wea.clear();

    Serial.println(weatherId);
    lastConnectionTime = millis();

  } else {

    return;
  }
}




void setBrightness(uint8_t value)
{
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;



  FastLED.setBrightness(value);

  EEPROM.write(0, value);
  EEPROM.commit();
}




void showBitmap()
{
  FastLED.setBrightness(EEPROM.read(0));
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
            if (durchgang >= EEPROM.read(1)) {
              durchgang = 0;
              nextPattern();
            }
          } else {
            if (durchgang >= EEPROM.read(1) * 80) {
              durchgang = 0;
              nextPattern();
            }
          }
        }

        else {

          gPatterns[gCurrentPatternNumber]();
          gHue++;
          if (gCurrentPatternNumber == 8 || gCurrentPatternNumber == 9 || gCurrentPatternNumber == 10 || gCurrentPatternNumber == 11) {
            if (rounds >= EEPROM.read(9) * 10) {
              rounds = 0;
              FastLED.clear();
              nextPattern();
            }
          }
          else if (gCurrentPatternNumber == 12 || gCurrentPatternNumber == 13) {
            if (rounds >= EEPROM.read(10) * 10) {
              rounds = 0;
              FastLED.clear();
              nextPattern();
            }
          }
        } 
      } else {   //GIF - ANIMATIONEN ////////////////////////////////////


        if (EEPROM.read(85) == 1) { // WETTER AKTIVIERT


          if (EEPROM.read(83) == 0) {
            if (durchgang >= EEPROM.read(1) + EEPROM.read(11)) {
              durchgang = 0;
              showGif();
            } else {
              if (durchgang >= EEPROM.read(1)) {
                showWeather();
              } else {
                Uhrzeit();
              }
            }
          } else {
            if (durchgang >= EEPROM.read(1) * 50 + EEPROM.read(11)) {
              durchgang = 0;
              showGif();
            } else {
              if (durchgang >= EEPROM.read(1) * 50) {
                showWeather();
              } else {
                Uhrzeit();
              }
            }
          }





        } else {    // WETTER DEAKTIVIERT



          if (EEPROM.read(83) == 0) {
            if (durchgang >= EEPROM.read(1)) {
              durchgang = 0;
              showGif();
            } else {
              Uhrzeit();
            }
          } else {
            if (durchgang >= EEPROM.read(1) * 50) {
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



void showText() {
  FastLED.setBrightness(EEPROM.read(0));
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
  FastLED.setBrightness(EEPROM.read(0));
  matrix->clear();
  fill_solid( leds, NUM_LEDS, CRGB(EEPROM.read(72), EEPROM.read(73), EEPROM.read(74)));
  matrix->show();
}



void showWeather() {



  if (millis() - lastConnectionTime > postingInterval) {
    updateWeather();
  }


  FastLED.setBrightness(EEPROM.read(0));
  matrix->clear();
  matrix->setTextColor(matrix->Color(255, 255, 255));
  matrix->setCursor(6, 14);
  matrix->setFont(&TomThumb);
  matrix->print(weatherTemperature);
  matrix->print("C");
  matrix->drawPixel(18, 9, CRGB(255, 255, 255));

  if (weatherId == 801 || weatherId == 802) {

    if (!partofday) {
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
    if (!partofday) {
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
    if (!partofday) {
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

  String fileN;
  int num = 0;
  Dir dir = SPIFFS.openDir("/gif");
  while (dir.next()) {
    num++;
  }

  if (num == 0) {

    return;
  }

  int ran = random(num);


  Dir folder = SPIFFS.openDir("/gif");

  for (int i = 0; i <= ran; i++) {


    Serial.println(i);
    folder.next();

    fileN = folder.fileName();

  }

  uint32_t result;



  File imageFile = SPIFFS.open(fileN, "r");
  if (!imageFile) {

    return;
  }

  gifPlayer.setFile(imageFile);

  for (uint8_t c = 0; c < EEPROM.read(100); c++) {                // Number of Repeats of the Gif-Animation
    if (!gifPlayer.parseGifHeader()) {
      imageFile.close();

      return;
    }

    matrix->clear();
    gifPlayer.parseLogicalScreenDescriptor();
    gifPlayer.parseGlobalColorTable();
    do {                                            // Very bad code!! needs to be a non-blocking function.
      gifPlayer.drawFrame();
      result = gifPlayer.drawFrame();
      matrix->show();
      delay(0);
    } while (result != ERROR_FINISHED);
    imageFile.seek(0);
  }


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
