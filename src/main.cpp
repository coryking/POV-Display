#include <SPI.h>

// #define FASTLED_ALL_PINS_HARDWARE_SPI
// #define FASTLED_ESP32_SPI_BUS HSPI

#include <FastLED.h>

// Pin definitions
#define HALL_PIN D0

#define LED_1_CLOCK D1 // Inside
#define LED_1_DATA D2
#define LED_2_CLOCK D3 // Middle
#define LED_2_DATA D4
#define LED_3_CLOCK D5 // Outside
#define LED_3_DATA D6

#define DEGREES_PER_COLUMN 1
#define NUM_ACTIVE_SEGMENTS 3 // Can be changed to 2 or 3 as needed

#define NUM_LEDS_PER_SEGMENT 10
#define NUM_SEGMENTS 3

#define LED_DATA_RATE_MHZ 24

#define DEBOUNCE_MICROS 100 // microseconds of debounce

#define NUM_LEDS (NUM_LEDS_PER_SEGMENT * NUM_ACTIVE_SEGMENTS)


CRGB leds[NUM_SEGMENTS][NUM_LEDS_PER_SEGMENT];

volatile bool triggerReceived = false;
volatile unsigned long lastHallTrigger = 0;
unsigned long deltaT = 0; // Time per column

// Adjusted: To track half rotations
volatile uint64_t halfRotationCount = 0;

CRGBPalette16 currentPalette; // Define your palette here

const uint64_t shiftTime = 1 * 1e5;

// Forward declaration of functions
void hallEffectTrigger();

void updateLEDs(int column, uint64_t rotation_count, uint64_t the_time_micros)
{
  static uint64_t last_update_time = 0;
  static int paletteShift = 0;

  uint64_t timeDelta = the_time_micros - last_update_time;
  FastLED.clear(); // Clear all LEDs

  // Shift the palette every second
  if(timeDelta >= shiftTime) {
    last_update_time = the_time_micros;
    paletteShift++;
  }

  // Define the angular offset for each segment
  int segmentOffset = 360 / NUM_SEGMENTS;

  // Map the entire palette across the circle with angular offset for each segment
  for (int segment = 0; segment < NUM_SEGMENTS; ++segment) {
    for (int i = 0; i < NUM_LEDS_PER_SEGMENT; ++i) {
      // Calculate the overall LED index considering the segment offset
      int overallIndex = segment * NUM_LEDS_PER_SEGMENT + i;
      // Adjust the palette index for the segment's angular offset
      int paletteIndex = (overallIndex * 255 / NUM_LEDS + paletteShift + segment * segmentOffset * 255 / 360) % 255;
      CRGB color = ColorFromPalette(currentPalette, paletteIndex);
      leds[segment][i] = color;
    }
  }

  FastLED.show(); // Update the display
}


void setup()
{
  // Set up the LED strip
  FastLED.addLeds<SK9822, LED_1_DATA, LED_1_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0], NUM_LEDS_PER_SEGMENT);
  FastLED.addLeds<APA102, LED_2_DATA, LED_2_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[1][0], NUM_LEDS_PER_SEGMENT);
  FastLED.addLeds<APA102, LED_3_DATA, LED_3_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[2][0], NUM_LEDS_PER_SEGMENT);

  FastLED.clear();
  FastLED.show();
  FastLED.setBrightness(30);
  Serial.begin();

  // Set up the Hall effect sensor interrupt
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallEffectTrigger, FALLING);

  currentPalette = RainbowColors_p;
}

int calculateCurrentColumn(unsigned long currentMicros, unsigned long triggerMicros, unsigned long timePerColumn)
{
  float elapsedMicros = currentMicros - triggerMicros;
  float columnsElapsed = elapsedMicros / timePerColumn;
  return static_cast<int>(round(columnsElapsed)); // Round to nearest column
}
void loop() {
  static unsigned long lastUpdateTime = 0;
  static int lastColumn = -1;

  if (triggerReceived) {
    unsigned long currentTime = micros();

    if (currentTime - lastHallTrigger > DEBOUNCE_MICROS) {
      if (lastHallTrigger != 0) {
        deltaT = (currentTime - lastHallTrigger) / (180 / DEGREES_PER_COLUMN); // Time per column for half revolution
      }

      lastHallTrigger = currentTime;
      triggerReceived = false;
      lastUpdateTime = lastHallTrigger;
      lastColumn = -1;
      halfRotationCount++; // Increment half rotation count on every trigger
    }
  }

  unsigned long currentMicros = micros();
  if (deltaT != 0 && (currentMicros - lastUpdateTime) >= deltaT) {
    int currentColumn = calculateCurrentColumn(currentMicros, lastHallTrigger, deltaT);

    // Adjust the current column for the second half of the rotation
    if (halfRotationCount % 2 == 1) { // Second half of the rotation
      currentColumn += 180 / DEGREES_PER_COLUMN;
    }

    if (currentColumn != lastColumn) {
      updateLEDs(currentColumn, halfRotationCount / 2, currentMicros); // Pass full rotation count
      lastColumn = currentColumn;
      lastUpdateTime = currentMicros;
    }
  }
}
void hallEffectTrigger() {
  triggerReceived = true;
  //Serial.print("Half Rotation: ");
  //Serial.println(halfRotationCount);
}