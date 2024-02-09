#include <Arduino.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <cassert>
#include "DisplayController.h"
#include "config.h"
#include "RTOSConfig.h"

DisplayController::DisplayController()
{
    renderer = new Renderer();
    generator = new SimpleLine();
}

void DisplayController::start()
{
    Serial.println("Starting displaycontroller");
    xTaskCreate(&DisplayController::StepHandlerTask, "TimingTask", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, &_stepTask);
    bufferManager = new FrameBufferManager_t<CRGB, 3, NUM_SEGMENTS, NUM_STEPS, NUM_LEDS_PER_SEGMENT>();
    _rotation_manager = new RotationManager(_stepTask, NUM_STEPS);
    _rotation_manager->start();
    generator->start();
    renderer->start();
    Serial.println("Everything is up");
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

        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE)
        {

            rotation_position_t pos = instance->_rotation_manager->getCurrentStep();
            Serial.printf("Hello from step task %d ts: %llu\n", pos.step, pos.stepTimestamp);
            instance->invokeRenderer(pos);
            instance->handleFrameShift(pos);
        }
    }
    vTaskDelete(NULL);
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
        Serial.println("Needs frame shift, eh?");
        bufferManager->shiftFrames();
        Serial.println("shifted");
        FrameBuffer *genBuffer = bufferManager->getGeneratorFrame();
        Serial.println("got frame to generate");
        genBuffer->clearBuffer();
        Serial.println("cleared buffer");
        generator->enqueueNextFrame({_rotation_manager->getEstTimeForFutureRotation(1), genBuffer});
        Serial.println("Enqueued stuff");
    }
}
