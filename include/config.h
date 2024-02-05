#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>

// Pin definitions
#define HALL_PIN D0

#define LED_1_CLOCK D1 // Inside
#define LED_1_DATA D2
#define LED_2_CLOCK D3 // Middle
#define LED_2_DATA D4
#define LED_3_CLOCK D5 // Outside
#define LED_3_DATA D6

#define DEGREES_PER_STEP 5
#define NUM_STEPS (360/DEGREES_PER_STEP)

#define NUM_ACTIVE_SEGMENTS 3 // Can be changed to 2 or 3 as needed

#define NUM_LEDS_PER_SEGMENT 10
#define NUM_SEGMENTS 3
#define NUM_ARMS 3

#define NUM_ROWS (NUM_LEDS_PER_SEGMENT * NUM_ACTIVE_SEGMENTS)

#define NUM_LEDS (NUM_ROWS * NUM_STEPS)

#define LED_DATA_RATE_MHZ 24

#define DEBOUNCE_MICROS 100 // microseconds of debounce

#define ARM_OFFSET (NUM_STEPS / NUM_ARMS)

#define RPM_SMOOTHING_LEVEL 10
#define NUM_MAGNETS 2

typedef float delta_t;
typedef uint64_t timestamp_t;

#endif // __CONFIG_H__