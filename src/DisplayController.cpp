#include <Arduino.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <cassert>
#include "DisplayController.h"
#include "RTOSConfig.h"

DisplayController::DisplayController()
{
    
}

void DisplayController::start()
{
     xTaskCreate(&DisplayController::StepHandlerTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_stepTask);   
}

TaskHandle_t DisplayController::getStepTaskHandler()
{
    return this->_stepTask;
}

void DisplayController::StepHandlerTask(void *pvParameters)
{
    DisplayController *instance = static_cast<DisplayController *>(pvParameters);
    assert(instance != nullptr);

    while (true)
    {
        instance->handleStep();
    }
    vTaskDelete(NULL);
}


void handleStep()
{
    
}
