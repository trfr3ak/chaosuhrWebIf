//  ***************************
//  *     Uhrzeit anzeigen    *
//  ***************************

void Uhrzeit() {

  FastLED.setBrightness(EEPROM.read(0));
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


  //FireClock - by Xenum
    if (EEPROM.read(83) == 3) {
    fireClock();
  }


  //Neoclock - by Xenum

  
  if (EEPROM.read(83) == 4) {
    neoClock();
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




//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// NEOCLOCK; FIRECLOCK - Credit: Xenum

boolean matrix_o[MATRIX_WIDTH * MATRIX_HEIGHT];
boolean invert = false;

uint16_t XY( uint8_t x, uint8_t y)
{
  if ( y & 0x01) return (y * MATRIX_WIDTH) + (MATRIX_WIDTH - 1) - x;
  return (y * MATRIX_WIDTH) + x;
}

boolean toSet (uint8_t x, uint8_t y) {
  return (matrix_o[y * MATRIX_WIDTH + x] != invert);
}

boolean bXY(uint8_t x, uint8_t y, CRGB value, byte scale) {
  if (toSet(x, y)) return leds[XY(x, y)] = value;
  return leds[XY(x, y)] =  value.nscale8(scale);
}

void setNum(byte digit, byte x, byte y) {
  matrix_o[x + 0 + y * MATRIX_WIDTH] = true;
  matrix_o[x + 1 + y * MATRIX_WIDTH] = true;
  matrix_o[x + 2 + y * MATRIX_WIDTH] = true;
  matrix_o[x + 0 + (y + 1) * MATRIX_WIDTH] = true;
  matrix_o[x + 2 + (y + 1) * MATRIX_WIDTH] = true;
  matrix_o[x + 0 + (y + 2) * MATRIX_WIDTH] = true;
  matrix_o[x + 1 + (y + 2) * MATRIX_WIDTH] = true;
  matrix_o[x + 2 + (y + 2) * MATRIX_WIDTH] = true;
  matrix_o[x + 0 + (y + 3) * MATRIX_WIDTH] = true;
  matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = true;
  matrix_o[x + 0 + (y + 4) * MATRIX_WIDTH] = true;
  matrix_o[x + 1 + (y + 4) * MATRIX_WIDTH] = true;
  matrix_o[x + 2 + (y + 4) * MATRIX_WIDTH] = true;
  switch (digit) {
    case 1:
      matrix_o[x + 1 + y * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + y * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 1) * MATRIX_WIDTH] = false;
      matrix_o[x + 1 + (y + 2) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 2) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      matrix_o[x + 1 + (y + 4) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 4) * MATRIX_WIDTH] = false;
      break;
    case 2:
      matrix_o[x + 2 + (y + 1) * MATRIX_WIDTH] = false;
      matrix_o[x + 0 + (y + 3) * MATRIX_WIDTH] = false;
      break;
    case 3:
      matrix_o[x + 2 + (y + 1) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      break;
    case 4:
      matrix_o[x + 1 + y * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      matrix_o[x + 1 + (y + 4) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 4) * MATRIX_WIDTH] = false;
      break;
    case 5:
      matrix_o[x + 0 + (y + 1) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      break;
    case 6:
      matrix_o[x + 0 + (y + 1) * MATRIX_WIDTH] = false;
      break;
    case 7:
      matrix_o[x + 2 + (y + 1) * MATRIX_WIDTH] = false;
      matrix_o[x + 1 + (y + 2) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 2) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      matrix_o[x + 1 + (y + 4) * MATRIX_WIDTH] = false;
      matrix_o[x + 2 + (y + 4) * MATRIX_WIDTH] = false;
      break;
    case 9:
      matrix_o[x + 2 + (y + 3) * MATRIX_WIDTH] = false;
      break;
    case 0:
      matrix_o[x + 1 + (y + 2) * MATRIX_WIDTH] = false;
      break;
  }
}

byte lastsec = 255;

void renderTime() {
  time_t now = time(&now);
  localtime_r(&now, &lt);
  if (lt.tm_sec != lastsec) {
    lastsec = lt.tm_sec;
    setNum(lt.tm_hour / 10, 15, 2);
    setNum(lt.tm_hour % 10, 11, 2);
    setNum(lt.tm_min / 10, 6, 2);
    setNum(lt.tm_min % 10, 2, 2);
    setNum(lt.tm_sec / 10, 11, 8);
    setNum(lt.tm_sec % 10, 6, 8);
  }
}

uint8_t fire_n[MATRIX_WIDTH][MATRIX_HEIGHT];
uint8_t fire_h[NUM_LEDS];

byte fire_scaleBackGround = 8;

void fireClock() {
  renderTime();
  uint16_t c1 = inoise16(11 * millis(), 0, 0);
  uint16_t c2 = inoise16(13 * millis(), 100000, 100000);
  uint16_t  c3 = ((c1 + c2) / 2);
  uint8_t s = 20;

  uint32_t x1 = 3 * c3 * s;
  uint32_t y1 = 15 * millis() * s;
  uint32_t z1 = 3 * millis() * s ;
  uint32_t sx = c1 / 2;
  uint32_t sy = c2 / 2;

  for (uint8_t i = 0; i < MATRIX_WIDTH; i++) {
    uint32_t io = sx * (i - (MATRIX_WIDTH / 2) - 1);
    for (uint8_t j = 0; j < MATRIX_HEIGHT; j++) {
      uint32_t jio = sy * (j - (MATRIX_HEIGHT / 2) - 1);
      uint16_t d = ((inoise16(x1 + io, y1 + jio, z1)) + 1);
      fire_n[i][j] = d >> 8;
    }
  }

  for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    bXY(x, MATRIX_HEIGHT - 1, ColorFromPalette( HeatColors_p, fire_n[x][0]), fire_scaleBackGround);
    fire_h[XY(x, MATRIX_HEIGHT - 1)] = fire_n[x][0];
  }

  for (uint8_t y = 0; y < MATRIX_HEIGHT - 1; y++)
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      fire_h[XY(x, y)] = fire_h[XY(x, y + 1)];
    }

  for (uint8_t y = 0; y < MATRIX_HEIGHT - 1; y++)
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t dim = fire_n[x][y];
      dim = 255 - dim / 1.8;
      fire_h[XY(x, y)] = scale8(fire_h[XY(x, y)] , dim);
    }

  for (uint8_t y = 0; y < MATRIX_HEIGHT - 1; y++)
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      bXY(x, y, ColorFromPalette( HeatColors_p, fire_h[XY(x, y)]), fire_scaleBackGround);
    }

  FastLED.show();
  FastLED.delay(10);
  rounds++;
}

CRGB mMap[MATRIX_WIDTH][MATRIX_HEIGHT];
byte bMap[MATRIX_WIDTH];
byte neo_scaleTrail = 192;
byte neo_scaleBackGround = 8;

void  neoClock() {
  renderTime();
  for (int8_t row = MATRIX_HEIGHT - 1; row >= 0; row--)
    for (int8_t col = 0; col < MATRIX_WIDTH; col++)
      if (mMap[col][row].r == 255 || mMap[col][row].g == 255 || mMap[col][row].b == 255)
      {
        if (row < MATRIX_HEIGHT - 1)
          mMap[col][row + 1] = mMap[col][row];
        else
          bMap[col]--;
        mMap[col][row] = CRGB(27, 130, 39);
      }

  for (int8_t row = MATRIX_HEIGHT - 1; row >= 0; row--)
    for (int8_t col = 0; col < MATRIX_WIDTH; col++) {
      if (mMap[col][row].r != 255 && mMap[col][row].g != 255 && mMap[col][row].b != 255) mMap[col][row].nscale8(neo_scaleTrail); // only fade trail
    }

  if (random8(3) == 0) // lower number == more frequent spawns
  {
    int8_t spawnX = random8(MATRIX_WIDTH);
    if (bMap[spawnX] < 4) {
      mMap[spawnX][0] = CRGB(175, 255, 175);
      bMap[spawnX] = bMap[spawnX] + 1;
    }
  }
  for (int8_t col = 0; col < MATRIX_WIDTH; col++)
    if (bMap[col] < 2 ) {
      mMap[col][0] = CRGB(175, 255, 175);
      bMap[col]++;
    }

  for (int8_t row = MATRIX_HEIGHT - 1; row >= 0; row--)
    for (int8_t col = 0; col < MATRIX_WIDTH; col++)
      bXY(col, row,  mMap[col][row], neo_scaleBackGround);
  FastLED.show();
  delay(50);
}
