#ifndef __DISPLAYCONTROLLER_H__
#define __DISPLAYCONTROLLER_H__
#include "FastLEDConfig.h"
#include "FrameBufferManager.h"
#include "Generator.h"
#include "Generators/SimpleLine.h"
#include "HallEffectDriver.h"
#include "RTOSConfig.h"
#include "Renderer.h"
#include "SegmentBuffer.h"
#include "StepIntervalCalculator.h"
#include "StepPulseGenerator.h"
#include "config.h"
#include "types.h"
#include <Arduino.h>

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
    QueueHandle_t _queueHandle;
    
    static void StepHandlerTask(void *pvParameters);
    void invokeRenderer(rotation_position_t rotationPosition);
    void handleFrameShift(rotation_position_t rotationPosition);

    HallEffectDriver *hallDriver;
    StepPulseGenerator *pulseDriver;
    StepIntervalCalculator *intervalCalculator;

    FrameBufferManager_t<CRGB, 3, NUM_SEGMENTS, NUM_STEPS, NUM_LEDS_PER_SEGMENT> *bufferManager;
    Renderer *renderer;
    Generator *generator;
};
#endif // __DISPLAYCONTROLLER_H__