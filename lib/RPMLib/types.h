#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>
#include <float.h>
#include <cmath>

typedef float delta_t;
typedef uint64_t timestamp_t;

typedef uint64_t micro_per_rev_t;

typedef float rpm_t;
typedef float motorspeed_t;

typedef int column_num_t;

typedef int32_t step_t;


typedef struct  {
    step_t step = -1;
    timestamp_t stepTimestamp;
} rotation_position_t;

/**
 * @brief compute the RPM based on the delta_t and number of magnets. delta_t is in microseconds
 * 
 */

#define RPM_FROM_DELTA_T_AND_MAGNETS(time_delta, num_magnets) ((time_delta) == 0.0 || (num_magnets) == 0.0 ? 0.0: static_cast<float>(60000000.0 / (static_cast<float>(time_delta) * static_cast<float>(num_magnets))))

#ifndef MICROSECONDS_TO_HZ
#define MICROSECONDS_TO_HZ(deltaMicroseconds) (1000000.0f / static_cast<float>(deltaMicroseconds))
#endif

#ifndef pdUS_TO_TICKS
    #define pdUS_TO_TICKS(xTimeInUs) ((TickType_t)(((uint64_t)(xTimeInUs) * (uint64_t)configTICK_RATE_HZ) / (uint64_t)1000000U))
#endif

//#define PID_TUNE_MODE

#endif // __TYPES_H__