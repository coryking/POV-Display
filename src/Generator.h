#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <Arduino.h>
#include "RTOSConfig.h" 
#include "types.h"
#include "FrameBuffer.h"

class Generator
{
public:
    Generator();
    bool start();
    QueueHandle_t getGeneratorQueueHandle();
    bool enqueueNextFrame(generatorParams params);

protected:
    virtual bool GenerateFrame(generatorParams params) = 0;

private:
    TaskHandle_t _generatorTask_h;
    QueueHandle_t _generatorQueue_h;
    static void GeneratorTaskShell(void *pvParameters);
};

#endif // __GENERATOR_H__