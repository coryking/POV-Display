#include <Arduino.h>
#include "RTOSConfig.h"
#include <cassert>
#include "DisplayController.h"
#include "config.h"
#include "RTOSConfig.h"
#include "esp_log.h"

static const char *TAG = "DisplayController";

DisplayController::DisplayController()
{
    renderer = new Renderer();
    generator = new SimpleLine();
    hallDriver = new HallEffectDriver(HALL_PIN);
    intervalCalculator = new StepIntervalCalculator(hallDriver->getEventQueue(), NUM_STEPS, NUM_MAGNETS);
    _queueHandle = xQueueCreate(10, sizeof(step_t)); // Adjust size as needed
}

void DisplayController::start()
{
    ESP_LOGI(TAG,"Starting displaycontroller");
    xTaskCreate(&DisplayController::StepHandlerTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_stepTask);
    if (_stepTask == NULL)
    {
        // Task creation failed
        ESP_LOGE(TAG, "Failed to create StepHandlerTask");
    }
    ESP_LOGI(TAG, "task created");
    bufferManager = new FrameBufferManager_t<CRGB, 3, NUM_SEGMENTS, NUM_STEPS, NUM_LEDS_PER_SEGMENT>();
    ESP_LOGI(TAG, "buffermanager created");
    pulseDriver = new StepPulseGenerator(_queueHandle, intervalCalculator, NUM_STEPS);
    ESP_LOGI(TAG, "pulsedriver created");
    hallDriver->start();
    ESP_LOGI(TAG, "halldriver done");
    intervalCalculator->start();
    ESP_LOGI(TAG, "intervalcalc done");
    pulseDriver->start();
    ESP_LOGI(TAG, "pulsedriver done");
    generator->start();
    ESP_LOGI(TAG, "generator done");
    renderer->start();
    ESP_LOGI(TAG, "Everything is up");
}

TaskHandle_t DisplayController::getStepTaskHandler()
{
    return this->_stepTask;
}

void DisplayController::StepHandlerTask(void *pvParameters)
{
    ESP_LOGD(TAG, "StepHandlerTask started");

    DisplayController *instance = static_cast<DisplayController *>(pvParameters);
    assert(instance != nullptr);

    step_t stepNumber;
    while (true)
    {
        if (xQueueReceive(instance->_queueHandle, &stepNumber, portMAX_DELAY) == pdPASS)
        {


                // Now, stepNumber contains the step that triggered the timer
                //ESP_LOGV(TAG, "Hello from step task %d\n", stepNumber);

                // Assuming you have a method to create a rotation_position_t from just a step number
                rotation_position_t pos(stepNumber, CURRENT_TIME_US());
                instance->invokeRenderer(pos);
                instance->handleFrameShift(pos);
            
        }
    }
}

void DisplayController::invokeRenderer(rotation_position_t rotationPosition)
{
    auto stepbuffer = bufferManager->getSegmentsForStep(rotationPosition.step);
    renderer->renderStepBuffer(stepbuffer);
}

void DisplayController::handleFrameShift(rotation_position_t rotationPosition)
{
    if (rotationPosition.step == NUM_STEPS-1)
    {
        bufferManager->shiftFrames();
        FrameBuffer *genBuffer = bufferManager->getGeneratorFrame();
        genBuffer->clearBuffer();
        generator->enqueueNextFrame({pulseDriver->estimateFutureStepZero(1), genBuffer});
    }
}
