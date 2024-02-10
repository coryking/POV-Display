#ifndef STEPINTERVALCALCULATOR_H
#define STEPINTERVALCALCULATOR_H

#include <stdint.h>

#include "RTOSConfig.h"
#include "freertos/queue.h"
#include "types.h"

#include "RPMLib.h"

class StepIntervalCalculator
{
  public:
    StepIntervalCalculator(QueueHandle_t sensorTriggerQueue, uint16_t stepsPerRotation, uint16_t triggersPerRotation);
    void start();

    stepInterval_t getCurrentStepInterval() const; // Returns the latest calculated interval between steps

    bool isDeviceRotating() const;
    bool isWarmUpComplete() const;

  private:
    const QueueHandle_t _sensorTriggerQueue; // Queue handle for receiving hall effect
    TaskHandle_t _taskHandle;                // Handle for the task that processes queue items

    const uint16_t _stepsPerRotation;    // Total steps in one full rotation
    const uint16_t _triggersPerRotation; // Number of hall effect sensor triggers per full rotation

    static bool deviceRotating;
    static bool warmUpComplete;
    static int triggerCount;

    static volatile stepInterval_t currentStepInterval; // Latest calculated step interval
    static timestamp_t lastTimestamp;                   // Last timestamp received for calculating intervals

    static void taskProcessor(void *pvParameters); // Task function for processing queue items

    void calculateStepInterval(timestamp_t currentTimestamp); // Calculates the step interval from timestamp data

    // Constants
    static const int triggerThreshold = 10;                      // Number of triggers for warm-up
    static const stepInterval_t maxIntervalForRotation = 100000; // 100 ms, equivalent to slower than 10 rotations/sec
};

#endif // STEPINTERVALCALCULATOR_H
