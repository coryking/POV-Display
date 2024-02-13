#include "StepPulseGenerator.h"
#include "config.h"
#include "esp_log.h"
#include <cmath>

#define MIN_STEP_INTERVAL 100
#define STEP_RETRY_MS 100

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
    if (!intervalCalculator->isWarmUpComplete() || !intervalCalculator->isDeviceRotating())
    {
        ESP_LOGD(TAG, "Not rotating or warm-up isn't complete... stalling");
        xTimerChangePeriod(timerHandle, pdMS_TO_TICKS(STEP_RETRY_MS), 0);
        return;
    }

    stepInterval_t nextInterval = intervalCalculator->getCurrentStepInterval();
    // Ensure there's a minimum interval to avoid 0 tick period
    nextInterval = max(nextInterval, static_cast<stepInterval_t>(MIN_STEP_INTERVAL));

    if (nextInterval > 0)
    {
        // Convert microseconds to ticks, ensuring the value is greater than 0
        TickType_t ticks = pdUS_TO_TICKS(nextInterval);
        ticks = ticks > 0 ? ticks : 1; // Ensure at least 1 tick
        xTimerChangePeriod(timerHandle, ticks, 0);
    }
    else
    {
        // Fallback in case of invalid interval
        ESP_LOGW(TAG, "Invalid interval calculated. Using fallback.");
        xTimerChangePeriod(timerHandle, pdMS_TO_TICKS(STEP_RETRY_MS), 0);
    }

    currentStep = (currentStep + 1) % stepsPerRotation;
    timestamp_t stepTimestamp = CURRENT_TIME_US();
    ESP_LOGD(TAG, "Ahoy current step is %d", currentStep);
    if (currentStep == 0)
    {
        updateStepZeroTimestamp(stepTimestamp);
    }
    xTaskNotify(targetTaskHandle, static_cast<uint32_t>(currentStep), eSetValueWithOverwrite);
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
