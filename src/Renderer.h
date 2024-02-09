#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "RTOSConfig.h"
#include "FastLEDConfig.h" 
#include "SegmentBuffer.h"
#include "types.h"

// NOTE: I'll sort out the template bullshit later. this is 100% pure CRGB for now.

class Renderer {
public:
    Renderer();
    void start();
    void renderStepBuffer(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT> buffer);

private:
    TaskHandle_t renderTaskHandle_h;
    MessageBufferHandle_t renderMessageBuffer_h;
    static void stepBufferRendererTask(void *pvParameters);
    CRGB leds[NUM_SEGMENTS][NUM_LEDS_PER_SEGMENT];

};

#endif // __RENDERER_H__