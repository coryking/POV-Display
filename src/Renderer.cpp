#include <Arduino.h>
#include "RTOSConfig.h"
#include "FastLEDConfig.h" 
#include <cassert>
#include "Renderer.h"
#include "config.h"
#include "esp_log.h"

static const char *TAG = "Renderer";

void Renderer::stepBufferRendererTask(void *pvParameters)
{
    Renderer *instance = (Renderer *)pvParameters;
    assert(instance != nullptr);

    while (true)
    {
        StepBuffer_t<CRGB, NUM_SEGMENTS,NUM_LEDS_PER_SEGMENT> stepBuffer;
        if (xMessageBufferReceive(instance->renderMessageBuffer_h, &stepBuffer, sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>), pdMS_TO_TICKS(20)))
        {
            ESP_LOGD(TAG, "Rendering Crap");
            FastLED.clear();
            for(int seg=0; seg < NUM_SEGMENTS; ++seg) {
                for(int led=0; led < NUM_LEDS_PER_SEGMENT; led++) {
                    instance->leds[seg][led] = stepBuffer[seg][led];
                }
            }
            FastLED.show();
            ESP_LOGD(TAG, "Done Crap");
        }
    }
}

Renderer::Renderer()
{
    renderMessageBuffer_h = xMessageBufferCreate(sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>));
}

void Renderer::start()
{
#ifdef ARDUINO_ARCH_RP2040
    FastLED.addLeds<SK9822, LED_1_DATA, LED_1_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0], NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_2_DATA, LED_2_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[1][0], NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_3_DATA, LED_3_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[2][0], NUM_LEDS_PER_SEGMENT);
#elif defined(ESP32)
    FastLED.addLeds<SK9822, LED_DATA, LED_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0], NUM_LEDS);
#endif
    xTaskCreate(&Renderer::stepBufferRendererTask, "Renderer", RTOS::XLARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY, NULL);
}

void Renderer::renderStepBuffer(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT> buffer)
{
    xMessageBufferSend(renderMessageBuffer_h, &buffer, sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>), pdMS_TO_TICKS(5));
}
