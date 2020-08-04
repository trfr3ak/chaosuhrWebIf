#ifndef config_h
#define config_h
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

//                  **********************
//                  *      Hostname      *
//                  **********************

#define HOSTNAME    "ChaosUhr"               // WLAN Name

//                 ***********************
//                 *Wetter Einstellungen *
//                 ***********************

#define WEATHER_API ""      //WetterAPI von weatherbit.io
#define WEATHER_LOCATION ""                     //Ort für Wetter


//                  *************************
//                  * Spotify Einstellungen *
//                  *************************

#define SPOTIFY_CLIENT_ID "" // Your client ID of your spotify APP
#define SPOTIFY_CLIENT_SECRET "" // Your client Secret of your spotify APP (Do Not share this!)

// Country code, including this is advisable
#define SPOTIFY_MARKET "DE"

#define SPOTIFY_REFRESH_TOKEN ""






//                  *************************
//                  * FastLED Einstellungen *
//                  *************************

#define DATA_PIN               D6                // Output PIN für Matrix (D6 für Wemos, für andere Boards evtl. nur die 6)
#define LED_TYPE               WS2812B           // LED Stripe TYP
#define COLOR_ORDER            GRB               // RGB oder GRB Anordung, sofern Farben nicht passen, hier ändern
#define FRAMES_PER_SECOND      150               // Wiederholrate
#define mw                     20                // LEDs in Matrixbreite
#define mh                     15                // LEDs in Matrixhöhe
#define NUM_LEDS               (mw*mh)           // Anzahl LEDs der Matrix
#define VOLTS                  5                 // Volt der LEDs/des Netzteils
#define MILLI_AMPS             9000              // Milliampere-Angabe des verwendeten Netzteils - wird verwendet um den maximalen Strom/Helligkeit zu ermitteln

#define MATRIX_WIDTH   20 
#define MATRIX_HEIGHT  15 

//  **************************
//  *   Definiere Matrix     *
//  **************************

CRGB leds[NUM_LEDS];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, mw, mh, 1, 1, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

//  **************************
//  * Definitionen Zeitzone  *
//  **************************

struct tm lt;         // http://www.cplusplus.com/reference/ctime/tm/
const char* const PROGMEM ntpServer[] = {"fritz.box", "de.pool.ntp.org", "at.pool.ntp.org", "ch.pool.ntp.org", "ptbtime1.ptb.de", "europe.pool.ntp.org"};
const char* const PROGMEM dayShortNames[] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
const char* const PROGMEM dayNames[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

#endif
