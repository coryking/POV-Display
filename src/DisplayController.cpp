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
}

void DisplayController::start()
{
    ESP_LOGI(TAG,"Starting displaycontroller");
    xTaskCreate(&DisplayController::StepHandlerTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_stepTask);
    bufferManager = new FrameBufferManager_t<CRGB, 3, NUM_SEGMENTS, NUM_STEPS, NUM_LEDS_PER_SEGMENT>();
    pulseDriver = new StepPulseGenerator(&_stepTask, intervalCalculator, NUM_STEPS);
    hallDriver->start();
    intervalCalculator->start();
    pulseDriver->start();
    generator->start();
    renderer->start();
    ESP_LOGI(TAG, "Everything is up");
}

TaskHandle_t DisplayController::getStepTaskHandler()
{
    return this->_stepTask;
}

void DisplayController::StepHandlerTask(void *pvParameters)
{
    DisplayController *instance = static_cast<DisplayController *>(pvParameters);
    assert(instance != nullptr);

    uint32_t stepNumber;
    while (true)
    {
        if (xTaskNotifyWait(0x00, ULONG_MAX, &stepNumber, portMAX_DELAY) == pdTRUE)
        {
            // Now, stepNumber contains the step that triggered the timer
            ESP_LOGV(TAG, "Hello from step task %d\n", stepNumber);

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
        ESP_LOGD(TAG, "Needs frame shift, eh?");
        bufferManager->shiftFrames();
        ESP_LOGV(TAG, "shifted");
        FrameBuffer *genBuffer = bufferManager->getGeneratorFrame();
        ESP_LOGV(TAG, "got frame to generate");
        genBuffer->clearBuffer();
        ESP_LOGV(TAG, "cleared buffer");
        generator->enqueueNextFrame({pulseDriver->estimateFutureStepZero(1), genBuffer});
        ESP_LOGD(TAG, "Done, Enqueued stuff");
    }
}
