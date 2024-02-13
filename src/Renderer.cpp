#include "Renderer.h"
#include "FastLEDConfig.h"
#include "RTOSConfig.h"
#include "config.h"
#include "esp_log.h"
#include <Arduino.h>
#include <cassert>

static const char *TAG = "Renderer";
CRGB leds[NUM_LEDS];
void Renderer::stepBufferRendererTask(void *pvParameters)
{
    ESP_LOGD(TAG, "hello from render");
    Renderer *instance = (Renderer *)pvParameters;
    assert(instance != nullptr);

    StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT> stepBuffer;

    while (true)
    {
        xMessageBufferReceive(instance->renderMessageBuffer_h, &stepBuffer,
                              sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>), pdMS_TO_TICKS(20));

        //ESP_LOGV(TAG, "Rendering Crap");
        FastLED.clear();
        for (int seg = 0; seg < NUM_SEGMENTS; ++seg)
        {
            for (int led = 0; led < NUM_LEDS_PER_SEGMENT; led++)
            {
                leds[led + seg * NUM_LEDS_PER_SEGMENT]  = stepBuffer[seg][led];
            }
        }
        FastLED.show();
        //ESP_LOGV(TAG, "Done Crap");
    }
}

Renderer::Renderer()
{
    renderMessageBuffer_h = xMessageBufferCreate(sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>));
}

void Renderer::start()
{
    ESP_LOGD(TAG, "Hello from render start");
#ifdef ARDUINO_ARCH_RP2040
    FastLED.addLeds<SK9822, LED_1_DATA, LED_1_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0][0],
                                                                                            NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_2_DATA, LED_2_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[1][0],
                                                                                            NUM_LEDS_PER_SEGMENT);
    FastLED.addLeds<APA102, LED_3_DATA, LED_3_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[2][0],
                                                                                            NUM_LEDS_PER_SEGMENT);
#elif defined(ESP32)
    //FastLED.addLeds<SK9822, LED_DATA, LED_CLOCK>(&leds[0], 10); // Use a small, fixed number

    FastLED.addLeds<SK9822, LED_DATA, LED_CLOCK, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(&leds[0], NUM_LEDS);
#endif
    ESP_LOGD(TAG, "LED set up");
    xTaskCreate(&Renderer::stepBufferRendererTask, "Renderer", RTOS::XLARGE_STACK_SIZE, this, RTOS::HIGH_PRIORITY,
                NULL);
    ESP_LOGD(TAG, "Task Created");
}

void Renderer::renderStepBuffer(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT> buffer)
{
    xMessageBufferSend(renderMessageBuffer_h, &buffer, sizeof(StepBuffer_t<CRGB, NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT>),
                       pdMS_TO_TICKS(5));
}
