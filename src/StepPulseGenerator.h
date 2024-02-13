#ifndef __STEPPULSEGENERATOR_H__
#define __STEPPULSEGENERATOR_H__

#include "RTOSConfig.h"
#include "StepIntervalCalculator.h" // Assume this exists and is adjusted as needed
#include "types.h"

class StepPulseGenerator
{
  public:
    StepPulseGenerator(QueueHandle_t targetQueueHandle, StepIntervalCalculator *intervalCalculator,
                       uint16_t stepsPerRotation);
    void start();
    void stop();

    timestamp_t estimateFutureStepZero(uint16_t revolutionsAhead);

  private:
    TimerHandle_t timerHandle;
    const QueueHandle_t targetQueueHandle;              // Task to notify
    StepIntervalCalculator *intervalCalculator; // For getting step intervals
    const uint16_t stepsPerRotation;
    volatile step_t currentStep;
    timestamp_t lastStepZeroTimestamp; // Timestamp of the last "step zero"

    static void timerCallback(TimerHandle_t xTimer);
    void notifyNextStep();
    void updateStepZeroTimestamp(timestamp_t timestamp); // Updates the timestamp at "step zero"
};

#endif // __STEPPULSEGENERATOR_H__