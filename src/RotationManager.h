#ifndef __ROTATIONMANAGER_H__
#define __ROTATIONMANAGER_H__

#include <task.h>
#include <cstdint>
#include "config.h"
#include "RPMLib.h"

#define IS_DIVISIBLE_BY_360(x) (360 % (x) == 0)


typedef RPMSmoother_t<RPM_SMOOTHING_LEVEL, NUM_MAGNETS> Smoother;

typedef void (*microperrev_cb)(micro_per_rev_t newMicrosPerRevolution);


class RotationManager
{
public:
    RotationManager(TaskHandle_t stepTriggerTask_h, column_num_t stepsPerRotation = 360);
    void start();
    delta_t getµsPerStep();

protected:
    static RotationManager *instance;

    static void isrWrapper()
    {
        assert(instance != nullptr);

        instance->HallEffectISR();
    
    }

    void HallEffectISR();

private:
    static void timingTask(void *pvParameters);
    
    static void stepTimer(TimerHandle_t xTimer);
    void adjustTimer();

    TaskHandle_t _timingTask_h;
    const TaskHandle_t _stepTriggerTask_h;
    TimerHandle_t _stepTimer_h;    
    Smoother* _rpm;
    const column_num_t _stepsPerRotation;
    volatile column_num_t _currentStep = -1;
    delta_t _µsPerStep;
    
};

#endif // __ROTATIONMANAGER_H__