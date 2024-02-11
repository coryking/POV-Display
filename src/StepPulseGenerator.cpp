#include "StepPulseGenerator.h"
#include "config.h"
#include "esp_log.h"

static const char *TAG = "DisplayController";

StepPulseGenerator::StepPulseGenerator(TaskHandle_t targetTaskHandle, StepIntervalCalculator *intervalCalculator,
                                       uint16_t stepsPerRotation)
    : targetTaskHandle(targetTaskHandle), intervalCalculator(intervalCalculator), stepsPerRotation(stepsPerRotation),
      currentStep(0)
{
    timerHandle = xTimerCreate("StepTimer", pdMS_TO_TICKS(1000), pdFALSE, this, timerCallback);
}

void StepPulseGenerator::start()
{
    xTimerStart(timerHandle, 0);
}

void StepPulseGenerator::stop()
{
    xTimerStop(timerHandle, 0);
}

void StepPulseGenerator::timerCallback(TimerHandle_t xTimer)
{
    StepPulseGenerator *generator = static_cast<StepPulseGenerator *>(pvTimerGetTimerID(xTimer));
    generator->notifyNextStep();
}

void StepPulseGenerator::notifyNextStep()
{
    if(!intervalCalculator->isWarmUpComplete() || !intervalCalculator->isDeviceRotating()) {
        // if the thing is stopped or not ready, lets just keep watching until something happens
        xTimerChangePeriod(timerHandle, pdMS_TO_TICKS(10), 0);
        ESP_LOGD(TAG, "not rotating or warm up isn't complete... stalling");
        return;
    }

    stepInterval_t nextInterval = intervalCalculator->getCurrentStepInterval();
    currentStep = (currentStep + 1) % stepsPerRotation;


    timestamp_t stepTimestamp = CURRENT_TIME_US();

    if (currentStep == 0)
    {
        updateStepZeroTimestamp(stepTimestamp);
    }
    // Notify the target task with the next step position (assumes currentStep fits into a 32-bit int, which is entirely reasonable)
    xTaskNotify(targetTaskHandle, static_cast<uint32_t>(currentStep), eSetValueWithOverwrite);

    // Restart the timer with the next interval
    xTimerChangePeriod(timerHandle, pdMS_TO_TICKS(nextInterval), 0);
}

void StepPulseGenerator::updateStepZeroTimestamp(timestamp_t timestamp)
{
    lastStepZeroTimestamp = timestamp;
}

timestamp_t StepPulseGenerator::estimateFutureStepZero(uint16_t revolutionsAhead)
{
    stepInterval_t fullRotationInterval = intervalCalculator->getCurrentStepInterval() * stepsPerRotation;
    return lastStepZeroTimestamp + (fullRotationInterval * revolutionsAhead);
}
