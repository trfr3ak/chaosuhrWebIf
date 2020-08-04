
//  ***************************
//  *    Figur Animationen    *
//  ***************************

void Drink() {
  int rounds = 0;
  while (rounds <= EEPROM.read(6)) {
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
  while (rounds <= EEPROM.read(8)) {
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
  while (rounds <= EEPROM.read(7)) {
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
  while (rounds < EEPROM.read(2)) {
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
  while (rounds < EEPROM.read(3)) {
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
  while (rounds < EEPROM.read(4)) {
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
  while (rounds < EEPROM.read(5)) {
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
  FastLED.setBrightness(EEPROM.read(0));
  fill_rainbow( leds, NUM_LEDS, gHue * 2, 7);
  FastLED.show();
  rounds++;
}

void rainbowWithGlitter() {  
  FastLED.setBrightness(EEPROM.read(0));    
  fill_rainbow( leds, NUM_LEDS, gHue*2, 7);
  addGlitter(50);  
  FastLED.show();   
  rounds++;
}

void addGlitter(fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }        
}

void confetti() {
  FastLED.setBrightness(EEPROM.read(0) * 2);
  fadeToBlackBy( leds, NUM_LEDS, 50);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue * 2 + random16(150), 210, 255);
  FastLED.show();
  rounds++;
}

void sinelon() {
  FastLED.setBrightness(EEPROM.read(0) * 2);
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = beatsin16( 25, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 210);
  FastLED.show();
  rounds++;
}

void bpm() {
  FastLED.setBrightness(EEPROM.read(0));
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
  FastLED.setBrightness(EEPROM.read(0));
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
