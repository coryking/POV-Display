#include "Generator.h"
#include <cassert>
#include "RTOSConfig.h"

Generator::Generator()
{
    _generatorQueue_h = xQueueCreate(1, sizeof(generatorParams));
}

bool Generator::start()
{
    xTaskCreate(Generator::GeneratorTaskShell, "GeneratorTask", RTOS::MEDIUM_STACK_SIZE, this, RTOS::NORMAL_PRIORITY, &_generatorTask_h);
    return true;
}

void Generator::GeneratorTaskShell(void *pvParameters)
{
    Generator *instance = static_cast<Generator *>(pvParameters);
    assert(instance != nullptr);

    while (true)
    {
        generatorParams params;
        if (xQueueReceive(instance->_generatorQueue_h, &params, portMAX_DELAY) == pdTRUE) {
            Serial.println("Asked to generate some crap. lets do it...");
            // validate that we are still in a good state for kicking off generation
            //if(params.framebuffer->getState() == FrameBufferState::QueuedForGeneration || params.framebuffer->getState() == FrameBufferState::Empty)
            //{
                // mark as generating
                params.framebuffer->setState(FrameBufferState::Generating);
                // okay buddy, generate that frame!
                auto result = instance->GenerateFrame(params);
                if(result) {
                    params.framebuffer->setState(FrameBufferState::ReadyToRender);
                }
            //}
        }
    }
    vTaskDelete(NULL);
}

QueueHandle_t Generator::getGeneratorQueueHandle()
{
    return _generatorQueue_h;
}

bool Generator::enqueueNextFrame(generatorParams params)
{
    // bail if this thing isn't empty
    //if(params.framebuffer->getState() != FrameBufferState::Empty)
    //    return false;

    // let the world know it is queued for generator
    params.framebuffer->setState(FrameBufferState::QueuedForGeneration);

    xQueueSend(_generatorQueue_h, &params, 0);
    return true;
}
