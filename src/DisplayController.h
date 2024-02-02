#ifndef __DISPLAYCONTROLLER_H__
#define __DISPLAYCONTROLLER_H__
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include "types.h"

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
    void handleStep();
};
#endif // __DISPLAYCONTROLLER_H__