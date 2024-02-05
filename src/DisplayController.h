#ifndef __DISPLAYCONTROLLER_H__
#define __DISPLAYCONTROLLER_H__
#include <Arduino.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <FastLED.h>
#include "RotationManager.h"
#include "FrameBufferManager.h"
#include "Renderer.h"
#include "Generator.h"
#include "SegmentBuffer.h"
#include "types.h"
#include "config.h"
#include "Generators/SimpleLine.h"

/**
 * @brief The Display Controller
 *
 * This dude is responsible for splitting up the timing signal and driving the renderage of the display
 *
 */
class DisplayController
{
public:
    DisplayController();
    void start();
    TaskHandle_t getStepTaskHandler();

private:
    TaskHandle_t _stepTask;
    static void StepHandlerTask(void *pvParameters);
    void invokeRenderer(rotation_position_t rotationPosition);
    void handleFrameShift(rotation_position_t rotationPosition);

    RotationManager* _rotation_manager;
    FrameBufferManager_t<CRGB>* bufferManager;
    Renderer* renderer;
    Generator* generator;



};
#endif // __DISPLAYCONTROLLER_H__