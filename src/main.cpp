#include <FastLED.h>

// Pin definitions
#define HALL_PIN D0

#define LED_1_CLOCK D1
#define LED_1_DATA  D2
#define LED_2_CLOCK D3
#define LED_2_DATA  D4
#define LED_3_CLOCK D5
#define LED_3_DATA  D6

#define DEGREES_PER_COLUMN 2
#define NUM_ACTIVE_SEGMENTS 1  // Can be changed to 2 or 3 as needed

#define NUM_LEDS_PER_SEGMENT 10
#define NUM_SEGMENTS 3

#define LED_DATA_RATE_MHZ 8




CRGB leds[NUM_SEGMENTS][NUM_LEDS_PER_SEGMENT];


volatile bool newRevolution = false;
volatile unsigned long lastHallTrigger = 0;
unsigned long deltaT = 0;  // Time for half a revolution (two triggers per full revolution)

// Forward declaration of functions
void hallEffectTrigger();

void updateLEDs(int column) {
  FastLED.clear();  // Clear all LEDs

  // Calculate which LEDs to light up based on the current column
  for (int segment = 0; segment < NUM_ACTIVE_SEGMENTS; ++segment) {
    for (int i = 0; i < NUM_LEDS_PER_SEGMENT; ++i) {
      // Determine the logic for lighting up LEDs based on the column
      // Example: light up the LED if it matches the column number
      if ((column / DEGREES_PER_COLUMN) % NUM_LEDS_PER_SEGMENT == i) {
        leds[segment][i] = CRGB::White;
      }
    }
  }

  FastLED.show(); // Update the display
}



void setup() {
  // Set up the LED strip
  FastLED.addLeds<APA102, LED_1_DATA, LED_1_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0], NUM_LEDS_PER_SEGMENT);
  FastLED.addLeds<APA102, LED_2_DATA, LED_2_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[1][0], NUM_LEDS_PER_SEGMENT);
  FastLED.addLeds<APA102, LED_3_DATA, LED_3_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[2][0], NUM_LEDS_PER_SEGMENT);

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
  return static_cast<int>(round(columnsElapsed)); // Round to nearest column
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
  Serial.println("hello");
}
