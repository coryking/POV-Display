#include <Arduino.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <cassert>

#include "RotationManager.h"
#include "config.h"
#include "RTOSConfig.h"

// Initialize the static member
RotationManager *RotationManager::instance = nullptr;


RotationManager::RotationManager(TaskHandle_t stepTriggerTask_h, column_num_t stepsPerRotation) : _stepTriggerTask_h(stepTriggerTask_h), _stepsPerRotation(stepsPerRotation)
{
    assert(IS_DIVISIBLE_BY_360(stepsPerRotation));

    instance=this;
    _rpm = new Smoother(micros());
    
}

void RotationManager::start()
{
    attachInterrupt(digitalPinToInterrupt(HALL_PIN), RotationManager::isrWrapper, FALLING);

    xTaskCreate(&RotationManager::timingTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_timingTask_h);
    this->_stepTimer_h = xTimerCreate("StepTimer", pdUS_TO_TICKS(1000), pdTRUE, this, &RotationManager::stepTimer);
}

delta_t RotationManager::getµsPerStep()
{
    return _µsPerStep;
}

void RotationManager::HallEffectISR()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Update the timestamp for RPM calculation
    this->_rpm->addTimestamp(micros());

#ifdef FORCE_COLUMNS_ON_HALL_SENSOR
    this->forceSteps();
#endif 

    // Notify the timing task that a new half-rotation has been triggered
    vTaskNotifyGiveFromISR(this->_timingTask_h, &xHigherPriorityTaskWoken);

    // Yield in case a higher priority task was woken by the ISR
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void RotationManager::timingTask(void *pvParameters)
{
    RotationManager *instance = static_cast<RotationManager *>(pvParameters);
    assert(instance != nullptr);

    while (true)
    {
        // wait until we get that juicy notification that it's time to do some computin'
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdPASS)
        {
            instance->adjustTimer();
        }
    }
    vTaskDelete(NULL);
}

void RotationManager::stepTimer(TimerHandle_t xTimer)
{
    RotationManager *instance = (RotationManager *)pvTimerGetTimerID(xTimer);
    assert(instance != nullptr);

    instance->_currentStep = (instance->_currentStep + 1) % instance->_stepsPerRotation;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // notify whatever task out there needs notification. give them the current step.
    xTaskNotifyFromISR(instance->_stepTriggerTask_h, instance->_currentStep, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void RotationManager::adjustTimer()
{
    this->_µsPerStep = _rpm->getMicrosecondsPerRevolution() / static_cast<delta_t>(_stepsPerRotation);

    xTimerChangePeriod(this->_stepTimer_h, pdUS_TO_TICKS(this->getµsPerStep()), 0);
}
