#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <cassert>
#include "Generator.h"
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
            // validate that we are still in a good state for kicking off generation
            if(params.framebuffer->getStatus() == FrameBufferStatus::QueuedForGeneration)
            {
                // mark as generating
                params.framebuffer->setStatus(FrameBufferStatus::Generating);
                // okay buddy, generate that frame!
                auto result = instance->GenerateFrame(params);
                if(result) {
                    params.framebuffer->setStatus(FrameBufferStatus::ReadyToRender);
                }
            }
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
    if(params.framebuffer->getStatus() != FrameBufferStatus::Empty)
        return false;

    // let the world know it is queued for generator
    params.framebuffer->setStatus(FrameBufferStatus::QueuedForGeneration);

    xQueueSend(_generatorQueue_h, &params, 0);
    return true;
}
