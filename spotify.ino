void handleSpotifyRoot() {
  /*
  char webpage[800];
  sprintf(webpage, "<a href=\"https://accounts.spotify.com/authorize?client_id=%s&response_type=code&redirect_uri=%s&scope=%s\">Spotify authentifizieren</a>", SPOTIFY_CLIENT_ID, callbackURI, "user-read-playback-state");
  server.send(200, "text/html", webpage);*/
}



void handleSpotifyCallback() {
  /*
  String code = "";
  const char *refreshToken = NULL;
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "code")
    {
      code = server.arg(i);
      refreshToken = spotify.requestAccessTokens(code.c_str(), callbackURI);
    }
  }

  if (refreshToken != NULL)
  {
    server.send(200, "text/plain", refreshToken);
  }
  else
  {
    server.send(404, "text/plain", F("Failed to load token, check serial monitor"));
  }
  */
}


void updateSpotify() {


/*
  if (millis() > requestDueTime) {

    CurrentlyPlaying currentlyPlaying = spotify.getCurrentlyPlaying(SPOTIFY_MARKET);

    spotifyTrack = currentlyPlaying.trackName;
    spotifyArtist = currentlyPlaying.firstArtistName;
    spotifyPlaying = currentlyPlaying.isPlaying;
    Serial.print(spotifyTrack);

    requestDueTime = millis() + delayBetweenRequests;
  }
*/
}


void showSpotify() {
  /*
  updateSpotify();
  if (spotifyPlaying) {
    FastLED.setBrightness(EEPROM.read(0));
    matrix->clear();
    matrix->setTextColor(matrix->Color(255, 255, 255));
    matrix->setCursor(0, 0);
    matrix->setFont(&TomThumb);
    matrix->print(spotifyTrack);
    matrix->drawPixel(18, 9, CRGB(255, 255, 255));
  }
*/
}
