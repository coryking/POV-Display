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

RotationManager::RotationManager(TaskHandle_t callbackHandle, uint numColumns) : _callbackTask_h(callbackHandle), _numColumns(numColumns), _degreesPerColumn(360 / numColumns)
{
    assert(IS_DIVISIBLE_BY_360(numColumns));

    instance = this;
    _rpm = new Smoother(micros());
}

void RotationManager::start()
{
    attachInterrupt(digitalPinToInterrupt(HALL_PIN), RotationManager::isrWrapper, FALLING);

    xTaskCreate(&RotationManager::timingTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_timingTask_h);
    this->_columnTimer_h = xTimerCreate("ColumnTimer", pdUS_TO_TICKS(1000), pdTRUE, this, &RotationManager::columnTimer);
}

delta_t RotationManager::getµsPerColumn()
{
    return _µsPerColumn;
}

void RotationManager::HallEffectISR()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Update the timestamp for RPM calculation
    this->_rpm->addTimestamp(micros());

#ifdef FORCE_COLUMNS_ON_HALL_SENSOR
    this->forceColumns();
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

void RotationManager::columnTimer(TimerHandle_t xTimer)
{
    RotationManager *instance = (RotationManager *)pvTimerGetTimerID(xTimer);
    assert(instance != nullptr);
    
    instance->_currentColumn = (instance->_currentColumn + 1) % instance->_numColumns;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(instance->_callbackTask_h, instance->_currentColumn, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void RotationManager::forceColumns()
{
    static bool isHalfColumn = false;

    // force the column count to match the position of the magnet (either 0 degrees or 180 degrees)
    if (isHalfColumn) {
        this->_currentColumn = this->_numColumns / 2; 
        isHalfColumn = !isHalfColumn;
    } else {
        this->_currentColumn = 0;
        isHalfColumn = !isHalfColumn;
    }

}

void RotationManager::adjustTimer()
{
    this->_µsPerColumn = _rpm->getMicrosecondsPerRevolution() / _numColumns;

    xTimerChangePeriod(this->_columnTimer_h, pdUS_TO_TICKS(this->getµsPerColumn()), 0);
}
