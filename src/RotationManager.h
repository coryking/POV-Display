#ifndef __ROTATIONMANAGER_H__
#define __ROTATIONMANAGER_H__

#include "RTOSConfig.h"

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
    delta_t getUsPerStep();
    rotation_position_t getCurrentStep();
    /**
     * @brief Get the estimated timestamp for n number of rotations in the future
     * 
     * @param rotationsOut how many rotations to estimate
     * @return timestamp_t estimated timestamp for this future rotation
     */
    timestamp_t getEstTimeForFutureRotation(uint rotationsOut);
    static RotationManager *instance;

protected:

    void setCurrentStep(step_t currentStep, timestamp_t stepTimestamp);

private:
    esp_err_t setupISR();

    static void IRAM_ATTR onHallSensorTriggerISR(void *args);

    static void timingTask(void *pvParameters);
    
    static void stepTimer(TimerHandle_t xTimer);
    void adjustTimer();

    TaskHandle_t _timingTask_h;
    const TaskHandle_t _stepTriggerTask_h;
    TimerHandle_t _stepTimer_h;    
    Smoother* _rpm;
    const step_t _stepsPerRotation;
    volatile rotation_position_t _rotation_position;
    delta_t _UsPerStep = 10.0;
    // The timestemp for the last step zero. used for estimating future timestamps
    volatile timestamp_t tsOfLastZeroPoint;
    
};

#endif // __ROTATIONMANAGER_H__