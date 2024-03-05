#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>
#include <Arduino.h>
#ifdef ARDUINO_ARCH_RP2040
// Pin definitions
#define HALL_PIN D0

#define LED_1_CLOCK D1 // Inside
#define LED_1_DATA D2
#define LED_2_CLOCK D3 // Middle
#define LED_2_DATA D4
#define LED_3_CLOCK D5 // Outside
#define LED_3_DATA D6
#elif defined(ESP32)
// all three segments are on the same strip
#define HALL_PIN D1
#define LED_CLOCK 7
#define LED_DATA 9
#endif

#define DEGREES_PER_STEP 20
#define NUM_STEPS (360/DEGREES_PER_STEP)

#define NUM_ACTIVE_SEGMENTS 3 // Can be changed to 2 or 3 as needed

#define NUM_LEDS_PER_SEGMENT 10
#define NUM_SEGMENTS 3
#define NUM_ARMS 3

#define NUM_ROWS (NUM_LEDS_PER_SEGMENT * NUM_ACTIVE_SEGMENTS)

#define NUM_LEDS (NUM_LEDS_PER_SEGMENT * NUM_SEGMENTS)

#define LED_DATA_RATE_MHZ 40

#define ARM_OFFSET (NUM_STEPS / NUM_ARMS)

#define RPM_SMOOTHING_LEVEL 20
#define NUM_MAGNETS 2

#define NUM_STEPS_BETWEEN_MAGNETS (NUM_STEPS / NUM_MAGNETS) // the number of steps between each magnet.


#ifdef ARDUINO_ARCH_RP2040
#define CURRENT_TIME_US() time_us_64()
#elif defined(ESP32)
#define CURRENT_TIME_US() esp_timer_get_time()
#endif

#endif // __CONFIG_H__