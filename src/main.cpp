#include <FastLED.h>

// Pin definitions
#define HALL_PIN D0
#define LED_PIN D1
#define NUM_LEDS 10

// Global variables
CRGB leds[NUM_LEDS];
volatile bool newRevolution = false;
volatile unsigned long lastHallTrigger = 0;
unsigned long deltaT = 0;  // Time for half a revolution (two triggers per full revolution)

// Forward declaration of functions
void hallEffectTrigger();

void updateLEDs(int column) {
  if(column == 0) {
    leds[0] = CRGB::Red;
    leds[4] = CRGB::Green;
    leds[9] = CRGB::Blue;
  } else {
    leds[0] = CRGB::Black;
    leds[4] = CRGB::Black;
    leds[9] = CRGB::Black;
  }
  FastLED.show();
}


void setup() {
  // Set up the LED strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  Serial.begin();

  // Set up the Hall effect sensor interrupt
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallEffectTrigger, FALLING);
}

int calculateCurrentColumn(unsigned long currentMicros, unsigned long triggerMicros, unsigned long timePerColumn) {
  float elapsedMicros = currentMicros - triggerMicros;
  float columnsElapsed = elapsedMicros / timePerColumn;
  return static_cast<int>(round(columnsElapsed)) * 2; // Multiply by 2 for full column count, round to nearest column
}


void loop() {
  static unsigned long lastUpdateTime = 0; // Last time the LEDs were updated
  static int lastColumn = -1; // Last column displayed

  // Check if a new revolution has started
  if (newRevolution) {
    unsigned long currentTime = micros();
    
    // Debounce check: Ensure enough time has passed since the last trigger to avoid bouncing
    if (currentTime - lastHallTrigger > 1000) { // Adjust the debounce time as needed
      // Calculate deltaT if it's not the first trigger
      if (lastHallTrigger != 0) {
        deltaT = (currentTime - lastHallTrigger) / 180;  // Time per half column (0.5 degrees)
      }
      // Reset for the next revolution
      lastHallTrigger = currentTime;
      newRevolution = false;
      lastUpdateTime = lastHallTrigger; // Sync the update time with the hall trigger
      lastColumn = -1; // Reset last column
    }
  }
  unsigned long currentMicros = micros();
  if (deltaT != 0 && (currentMicros - lastUpdateTime) >= deltaT) {
    int currentColumn = calculateCurrentColumn(currentMicros, lastHallTrigger, deltaT);
    if (currentColumn != lastColumn) {
      // Update the LEDs for the current column
      updateLEDs(currentColumn);
      lastColumn = currentColumn; // Update the last column
      lastUpdateTime = currentMicros; // Update the last update time
    }
  }
}

// Interrupt service routine for the Hall effect sensor
void hallEffectTrigger() {
  newRevolution = true;
  //Serial.println("hello");
}
