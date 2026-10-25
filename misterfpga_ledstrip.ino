#include <Adafruit_NeoPixel.h>
#include <math.h>

#define LED_PIN     3
#define NUM_LEDS    46

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ======= Konfigurierbare Variablen ======= //
uint8_t red   = 0;         // Rotwert (0–255)
uint8_t green = 0;         // Grünwert (0–255)
uint8_t blue  = 0;         // Blauwert (0–255)

uint8_t brightness = 255;   // Starthelligkeit (0–255)

// Knight Rider Effekt
uint8_t knightSpeed = 30;     // Geschwindigkeit in ms
uint8_t knightRepeats = 1;    // Anzahl der Wiederholungen

// Puls-Effekt
bool enablePulse = true;             // Pulsieren aktivieren oder deaktivieren
uint8_t pulseMinBrightness = 10;     // minimale Helligkeit
uint8_t pulseMaxBrightness = 100;     // maximale Helligkeit
uint16_t pulseInterval = 70;         // Puls-Update-Geschwindigkeit (ms)
// ========================================= //

void setup() {
  strip.begin();
  strip.setBrightness(brightness);
  strip.show();

  // Initialisiere den Zufallszahlengenerator mit einer zufälligen Quelle
  randomSeed(analogRead(0));  // Nutzen des analogen Pins 0 (ungenerell verbunden) als Seed

  // Zufällige Farbauswahl (rot, grün oder blau)
  chooseRandomColor();

  knightRiderEffect(knightRepeats, knightSpeed);  // Knight Rider Effekt starten
  fadeInLeds(red, green, blue, pulseMaxBrightness-40);  // LEDs mit der maximalen Helligkeit einblenden
}

void loop() {
  if (enablePulse) {
    pulseColor(red, green, blue);  // Pulsieren starten
  } else {
    fillColor(red, green, blue);  // Farbe statisch setzen
    delay(1000);
  }
}

// Funktion zur zufälligen Farbauswahl (rot, grün oder blau)
void chooseRandomColor() {
  int colorChoice = random(3);  // Generiert eine Zahl zwischen 0 und 2
  switch (colorChoice) {
    case 0:
      red = 255;   // Rot
      green = 0;   // Grün
      blue = 0;    // Blau
      break;
    case 1:
      red = 0;     // Rot
      green = 255; // Grün
      blue = 0;    // Blau
      break;
    case 2:
      red = 0;     // Rot
      green = 0;   // Grün
      blue = 255;  // Blau
      break;
  }
}

// Funktion für Knight Rider Effekt mit Fading-Außenkanten
void knightRiderEffect(uint8_t loops, uint8_t speedDelay) {
  const uint32_t baseColor = strip.Color(red, green, blue); // Hauptfarbe für Effekt

  for (uint8_t j = 0; j < loops; j++) {
    // Vorwärts
    for (int i = 0; i < NUM_LEDS; i++) {
      clearStrip();
      applyGradient(i, baseColor);
      strip.show();
      delay(speedDelay);
    }
    // Rückwärts
    for (int i = NUM_LEDS - 2; i >= 0; i--) {
      clearStrip();
      applyGradient(i, baseColor);
      strip.show();
      delay(speedDelay);
    }
  }
}

// Anwenden des Gradienten um eine zentrale LED
void applyGradient(int center, uint32_t color) {
  int r = (color >> 16) & 0xFF;
  int g = (color >> 8) & 0xFF;
  int b = color & 0xFF;

  for (int offset = -3; offset <= 3; offset++) {
    int pos = center + offset;
    if (pos >= 0 && pos < NUM_LEDS) {
      float brightnessFactor = 1.0 - (abs(offset) * 0.25); // 0.25 = starker Abfall, 3 LEDs Reichweite
      brightnessFactor = max(0.0, brightnessFactor);
      strip.setPixelColor(pos, strip.Color(r * brightnessFactor, g * brightnessFactor, b * brightnessFactor));
    }
  }
}

// Funktion um alle LEDs sanft einzublenden
void fadeInLeds(uint8_t r, uint8_t g, uint8_t b, uint8_t targetBrightness) {
  float scale = targetBrightness / 255.0;
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r * scale, g * scale, b * scale));
    strip.show();
    delay(knightSpeed);  // gleiche Geschwindigkeit wie Knight Rider
  }
}

// Pulsierender Farbeffekt (smooth per Sinuskurve)
void pulseColor(uint8_t r, uint8_t g, uint8_t b) {
  static float angle = 0.0;
  float brightnessFactor = (sin(angle) + 1.0) / 2.0; // Wert zwischen 0.0 und 1.0
  uint8_t brightnessValue = pulseMinBrightness + (brightnessFactor * (pulseMaxBrightness - pulseMinBrightness));

  fillColorWithBrightness(r, g, b, brightnessValue);  // Setze LEDs mit der berechneten Helligkeit

  angle += 0.05; // Schrittweite für weichen Übergang
  if (angle >= 2 * PI) angle -= 2 * PI;

  delay(pulseInterval);
}

// Farbe mit Helligkeit setzen
void fillColorWithBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t brightnessValue) {
  float scale = brightnessValue / 255.0;
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r * scale, g * scale, b * scale));
  }
  strip.show();
}

// Hilfsfunktion: Strip löschen
void clearStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);
  }
}

// Funktion um alle LEDs auf eine feste Farbe zu setzen
void fillColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}
