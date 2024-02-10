#ifndef HALLEFFECTDRIVER_H
#define HALLEFFECTDRIVER_H

#include "RTOSConfig.h"
#include <stdint.h>
#include "types.h"
#include "config.h"

// this should be 1
const uint8_t hallQueueLength = 1;

typedef struct
{
    timestamp_t triggerTimestamp;
} HallEffectEvent;

class HallEffectDriver
{
public:
    HallEffectDriver(uint8_t sensorPin = HALL_PIN);
    void start(); // Preferred method name for initialization

    static void IRAM_ATTR sensorTriggered_ISR(void *arg); // ISR for the hall effect sensor
    QueueHandle_t getEventQueue() const;              // Exposes the queue handle

private:
    const uint8_t _sensorPin;                      // GPIO pin for the hall effect sensor
    static QueueHandle_t _triggerEventQueue; // Queue for sensor trigger events
    esp_err_t setupISR();

};

#endif // HALLEFFECTDRIVER_H
