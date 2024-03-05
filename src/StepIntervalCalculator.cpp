#include "StepIntervalCalculator.h"
#include "HallEffectDriver.h"

static const char *TAG = "StepInterval";

StepIntervalCalculator::StepIntervalCalculator(QueueHandle_t sensorTriggerQueue, uint16_t stepsPerRotation,
                                               uint16_t triggersPerRotation)
    : _sensorTriggerQueue(sensorTriggerQueue), _stepsPerRotation(stepsPerRotation),
      _triggersPerRotation(triggersPerRotation)
{
}

void StepIntervalCalculator::start()
{
    _rpmLib = new RPMSmoother_t<RPM_SMOOTHING_LEVEL, NUM_MAGNETS>(CURRENT_TIME_US());

    xTaskCreate(taskProcessor, "StepIntervalTask", RTOS::XLARGE_STACK_SIZE, this, tskIDLE_PRIORITY, &_taskHandle);
}

stepInterval_t StepIntervalCalculator::getCurrentStepInterval() const
{
    if (!isWarmUpComplete())
    {
        return 0; // Or another predefined invalid value
    }

    if (!isDeviceRotating())
    {
        return UINT64_MAX;
    }
    return static_cast<stepInterval_t>(_rpmLib->getDelta()) / NUM_STEPS_BETWEEN_MAGNETS;
}

void StepIntervalCalculator::taskProcessor(void *pvParameters)
{
    auto *calculator = static_cast<StepIntervalCalculator *>(pvParameters);
    HallEffectEvent event;

    while (true)
    {
        if (xQueueReceive(calculator->_sensorTriggerQueue, &event, portMAX_DELAY) == pdPASS)
        {
            // Process event to update _currentStepInterval
            calculator->calculateStepInterval(event.triggerTimestamp);
        }
    }

}

void StepIntervalCalculator::calculateStepInterval(timestamp_t currentTimestamp)
{
    if (lastTimestamp != 0)
    {
        _rpmLib->addTimestamp(currentTimestamp);
        stepInterval_t interval = currentTimestamp - lastTimestamp;
        
        // Check for warm-up completion
        if (++triggerCount >= triggerThreshold)
        {
            warmUpComplete = true;
        }

        // Check if the device is rotating
        if (interval > maxIntervalForRotation)
        {
            deviceRotating = false;
        }
        else
        {
            currentMagnetInterval = interval; // Update interval after warm-up
            deviceRotating = true;
        }
    }
    lastTimestamp = currentTimestamp;
}

bool StepIntervalCalculator::isDeviceRotating() const
{
    return true;//deviceRotating;
}

bool StepIntervalCalculator::isWarmUpComplete() const
{
    return warmUpComplete;
}

volatile stepInterval_t StepIntervalCalculator::currentMagnetInterval = 0;
bool StepIntervalCalculator::warmUpComplete = false;
bool StepIntervalCalculator::deviceRotating = false;
timestamp_t StepIntervalCalculator::lastTimestamp = 0;
int StepIntervalCalculator::triggerCount = 0;
