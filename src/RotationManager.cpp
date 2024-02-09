#include <Arduino.h>
#include "RTOSConfig.h"

#include <cassert>

#include "RotationManager.h"
#include "config.h"
#include "esp_log.h"

static const char *TAG = "RotationManager";

RotationManager::RotationManager(TaskHandle_t stepTriggerTask_h, column_num_t stepsPerRotation) : _stepTriggerTask_h(stepTriggerTask_h), _stepsPerRotation(stepsPerRotation)
{
    assert(IS_DIVISIBLE_BY_360(stepsPerRotation));

    _rpm = new Smoother(micros());
    
}

void RotationManager::start()
{
    //attachInterrupt(digitalPinToInterrupt(HALL_PIN), isrHandler, FALLING);

    xTaskCreate(&RotationManager::timingTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_timingTask_h);
    this->_stepTimer_h = xTimerCreate("StepTimer", pdUS_TO_TICKS(1000), pdTRUE, this, &RotationManager::stepTimer);
    adjustTimer();
    ESP_LOGI(TAG,"Done with rotationmanager start");
}

rotation_position_t RotationManager::getCurrentStep()
{
    return rotation_position_t(_rotation_position.step, _rotation_position.stepTimestamp);
}

timestamp_t RotationManager::getEstTimeForFutureRotation(uint rotationsOut)
{
    return this->tsOfLastZeroPoint + this->_rpm->getMicrosecondsPerRevolution() * rotationsOut;
}

void RotationManager::setCurrentStep(step_t currentStep, timestamp_t stepTimestamp)
{
    static portMUX_TYPE currentStepMUX = portMUX_INITIALIZER_UNLOCKED;

    taskENTER_CRITICAL(&currentStepMUX);
    _rotation_position.step = currentStep;
    _rotation_position.stepTimestamp = stepTimestamp;
    taskEXIT_CRITICAL(&currentStepMUX);
}

esp_err_t RotationManager::setupISR()
{
    gpio_num_t pin = static_cast<gpio_num_t>(HALL_PIN);

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << HALL_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    intr_handle_t gpio_isr_handle; // Declare the handle

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(pin, &RotationManager::onHallSensorTriggerISR, this));

    return ESP_OK;
}

void IRAM_ATTR RotationManager::onHallSensorTriggerISR(void *args)
{
    RotationManager *instance = static_cast<RotationManager *>(args);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Update the timestamp for RPM calculation
    instance->_rpm->addTimestamp(CURRENT_TIME_US());

    // Notify the timing task that a new half-rotation has been triggered
    vTaskNotifyGiveFromISR(instance->_timingTask_h, &xHigherPriorityTaskWoken);

    instance->_UsPerStep = instance->_rpm->getMicrosecondsPerRevolution() / static_cast<delta_t>(instance->_stepsPerRotation);

    xTimerChangePeriodFromISR(instance->_stepTimer_h, pdUS_TO_TICKS(instance->getUsPerStep()), 0);

    // Yield in case a higher priority task was woken by the ISR
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

delta_t RotationManager::getUsPerStep()
{
    return _UsPerStep;
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

    instance->_rotation_position.step = (instance->_rotation_position.step + 1) % NUM_STEPS;
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

    this->_UsPerStep = _rpm->getMicrosecondsPerRevolution() / static_cast<delta_t>(_stepsPerRotation);
    ESP_LOGI(TAG, "Setting timer to %f", this->getUsPerStep());
    xTimerChangePeriod(this->_stepTimer_h, pdUS_TO_TICKS(this->getUsPerStep()), 0);
}
