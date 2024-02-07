#include <Arduino.h>
#include <FreeRTOS.h>
#include <FastLED.h>
#include <cassert>
#include "Renderer.h"
#include "config.h"
#include "RTOSConfig.h"

void Renderer::stepBufferRendererTask(void *pvParameters)
{
    Renderer *instance = (Renderer *)pvParameters;
    assert(instance != nullptr);

    while (true)
    {
        StepBuffer_t<CRGB> stepBuffer;
        if (xMessageBufferReceive(instance->renderMessageBuffer_h, &stepBuffer, sizeof(StepBuffer_t<CRGB>), pdMS_TO_TICKS(20)))
        {
            FastLED.clear();
            for(int seg=0; seg < _NUM_SEGMENTS; ++seg) {
                for(int led=0; led < _NUM_LEDS_PER_SEGMENT; led++) {
                    instance->leds[seg][led] = stepBuffer[seg][led];
                }
            }
            FastLED.show();
        }
    }
}

Renderer::Renderer()
{
    renderMessageBuffer_h = xMessageBufferCreate(sizeof(StepBuffer_t<CRGB>));
}

void Renderer::start()
{
    FastLED.addLeds<SK9822, LED_1_DATA, LED_1_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0], _NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_2_DATA, LED_2_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[1][0], _NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_3_DATA, LED_3_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[2][0], _NUM_LEDS_PER_SEGMENT);
    xTaskCreate(&Renderer::stepBufferRendererTask, "Renderer", RTOS::LARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, NULL);

}

void Renderer::renderStepBuffer(StepBuffer_t<CRGB> buffer)
{
    xMessageBufferSend(renderMessageBuffer_h, &buffer, sizeof(StepBuffer_t<CRGB>), pdMS_TO_TICKS(5));
}
