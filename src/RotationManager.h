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
    RotationManager(TaskHandle_t callbackTask, uint numColumns = 360);
    void start();
    delta_t getµsPerColumn();

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
    
    static void columnTimer(TimerHandle_t xTimer);

    void forceColumns();

    void adjustTimer();

    TaskHandle_t _timingTask_h;
    TaskHandle_t _callbackTask_h;
    TimerHandle_t _columnTimer_h;    
    Smoother* _rpm;
    const uint _numColumns;
    const uint _degreesPerColumn;
    volatile column_num_t _currentColumn = -1;
    volatile uint64_t _halfColumnCount = -1;
    delta_t _µsPerColumn;
    
};

#endif // __ROTATIONMANAGER_H__