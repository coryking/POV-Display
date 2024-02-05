#ifndef __RPM_H__
#define __RPM_H__

#include <cstdint>
#include "ExponentialMovingAverage.h"
#include "RollingAverage.h"

#include "types.h"


template<size_t _NUM_REVOLUTIONS, size_t _NUM_MAGENTS>
class RPMSmoother_t
{
public:
    RPMSmoother_t(timestamp_t currentTimestamp) : _lastTimestamp(currentTimestamp), _avg(0.1)
    {
    }

    RPMSmoother_t() : _lastTimestamp(0)
    {}

    RPMSmoother_t &addTimestamp(timestamp_t currentTimestamp)
    {
        delta_t delta = currentTimestamp - _lastTimestamp;
        _lastTimestamp = currentTimestamp;
        _avg_delta =_avg.add(delta).average();
        return *this;
    }

    /**
     * @brief Get the smoothed time delta between magnet triggers
     * 
     * @return delta_t 
     */
    delta_t getDelta()
    {
        return 1000 * 1000 * 2;
        //return _avg_delta;
    }

    /**
     * @brief Get the smoothed Microseconds Per Revolution
     * 
     * @return micro_per_rev_t 
     */
    micro_per_rev_t getMicrosecondsPerRevolution() {
        return getDelta() * _NUM_MAGENTS;
    }

    rpm_t getRPM() {
        return RPM_FROM_DELTA_T_AND_MAGNETS(getDelta(), _NUM_MAGENTS);
    }

private:
    // one sample per magnet * number of revolutions to average over...
    //RollingAverage<delta_t, _NUM_REVOLUTIONS * _NUM_MAGENTS> _avg;
    ExponentialMovingAverage<delta_t> _avg;
    volatile timestamp_t _lastTimestamp; // gotta keep this shit volalitile 'cause it will probably get messed with in some ISR
    delta_t _avg_delta = 0.0;
};

#endif // __RPM_H__