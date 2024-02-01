#ifndef __EXPONENTIALMOVINGAVERAGE_H__
#define __EXPONENTIALMOVINGAVERAGE_H__

#include <cstring> // For memset
#include <type_traits> // For std::is_arithmetic

/**
 * @brief Class to calculate the Exponential Moving Average (EMA) of incoming samples.
 *
 * @tparam T Numeric type of the data.
 */
template<typename T>
class ExponentialMovingAverage {
    static_assert(std::is_arithmetic<T>::value, "ExponentialMovingAverage requires a numeric data type");

private:
    T alpha; // Smoothing factor
    T filteredValue; // Current filtered value
    bool initialized; // Flag to check if the first value has been set

public:
    /**
     * @brief Constructs a new Exponential Moving Average object.
     *
     * @param alpha The smoothing factor (between 0 and 1).
     */
    ExponentialMovingAverage(T alpha) : alpha(alpha), filteredValue(0.0), initialized(false) {}

    /**
     * @brief Adds a new value to the average, updating the internal state.
     *
     * @param value The new value to add.
     * @return ExponentialMovingAverage& Reference to the updated object, for method chaining.
     */
    ExponentialMovingAverage& add(T value) {
        if (!initialized) {
            filteredValue = value; // Initialize with the first value
            initialized = true;
        } else {
            filteredValue = alpha * value + (1.0 - alpha) * filteredValue;
        }
        return *this;
    }

    /**
     * @brief Gets the current exponential moving average value.
     *
     * @return T The current average value.
     */
    T average() const {
        return filteredValue;
    }

    /**
     * @brief Resets the EMA to its initial state.
     */
    void reset() {
        filteredValue = 0.0;
        initialized = false;
    }
};

#endif // __EXPONENTIALMOVINGAVERAGE_H__