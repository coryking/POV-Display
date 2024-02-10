#ifndef __ROTATIONMANAGER_H__
#define __ROTATIONMANAGER_H__

#include "RTOSConfig.h"

#include <cstdint>
#include "config.h"
#include "RPMLib.h"

#define IS_DIVISIBLE_BY_360(x) (360 % (x) == 0)
#define MIN_DELTA_US 10 // minimum time between half rotations

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
    void adjustTimerToLastISR();

    const TaskHandle_t _stepTriggerTask_h;
    const step_t _stepsPerRotation;
    TimerHandle_t _stepTimer_h;
    TaskHandle_t _timingTask_h;
    Smoother *_rpm;

    delta_t _UsPerStep = 10.0;
    volatile rotation_position_t _rotation_position;
    // The timestemp for the last step zero. used for estimating future timestamps
    volatile timestamp_t tsOfLastZeroPoint;
    volatile timestamp_t lastISR;
};

#endif // __ROTATIONMANAGER_H__