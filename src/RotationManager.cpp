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

static void isrHandler() {
    RotationManager::instance->HallEffectISR();
}

RotationManager::RotationManager(TaskHandle_t stepTriggerTask_h, column_num_t stepsPerRotation) : _stepTriggerTask_h(stepTriggerTask_h), _stepsPerRotation(stepsPerRotation)
{
    assert(IS_DIVISIBLE_BY_360(stepsPerRotation));

    instance=this;
    _rpm = new Smoother(micros());
    
}

void RotationManager::start()
{
    //attachInterrupt(digitalPinToInterrupt(HALL_PIN), isrHandler, FALLING);

    xTaskCreate(&RotationManager::timingTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_timingTask_h);
    this->_stepTimer_h = xTimerCreate("StepTimer", pdUS_TO_TICKS(1000), pdTRUE, this, &RotationManager::stepTimer);
    adjustTimer();
    Serial.println("Done with rotationmanager start");
}

rotation_position_t RotationManager::getCurrentStep()
{
    rotation_position_t temp = {_rotation_position.step, _rotation_position.stepTimestamp};
    return temp;
}

timestamp_t RotationManager::getEstTimeForFutureRotation(uint rotationsOut)
{
    return this->tsOfLastZeroPoint + this->_rpm->getMicrosecondsPerRevolution() * rotationsOut;
}

void RotationManager::setCurrentStep(step_t currentStep, timestamp_t stepTimestamp)
{
    taskENTER_CRITICAL();
    _rotation_position.step = currentStep;
    _rotation_position.stepTimestamp = stepTimestamp;
    taskEXIT_CRITICAL();
}

delta_t RotationManager::getµsPerStep()
{
    return _µsPerStep;
}

void RotationManager::HallEffectISR()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Update the timestamp for RPM calculation
    this->_rpm->addTimestamp(time_us_64());

    // Notify the timing task that a new half-rotation has been triggered
    //vTaskNotifyGiveFromISR(this->_timingTask_h, &xHigherPriorityTaskWoken);

    this->_µsPerStep = _rpm->getMicrosecondsPerRevolution() / static_cast<delta_t>(_stepsPerRotation);

    xTimerChangePeriodFromISR(this->_stepTimer_h, pdUS_TO_TICKS(this->getµsPerStep()), 0);

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
    timestamp_t triggerTime = micros();

    UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    instance->_rotation_position.step = (instance->_rotation_position.step + 1) % _NUM_STEPS;
    instance->_rotation_position.stepTimestamp = triggerTime;

    // catch step zero so we can use it for future timestamp estimations
    if(instance->_rotation_position.step == 0) {
        instance->tsOfLastZeroPoint = triggerTime;
    }

    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // notify whatever task out there needs notification. give them the current step.
    vTaskNotifyGiveFromISR(instance->_stepTriggerTask_h, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void RotationManager::adjustTimer()
{

    this->_µsPerStep = _rpm->getMicrosecondsPerRevolution() / static_cast<delta_t>(_stepsPerRotation);

    xTimerChangePeriod(this->_stepTimer_h, pdUS_TO_TICKS(this->getµsPerStep()), 0);
}
