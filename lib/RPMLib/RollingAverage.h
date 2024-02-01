#ifndef ROLLING_AVERAGE_H
#define ROLLING_AVERAGE_H

#include <cstring> // For memset
#include <type_traits> // For std::is_arithmetic

/**
 * @brief Template class to calculate the rolling average of a fixed number of samples.
 *
 * @tparam T Numeric type of the data.
 * @tparam N Number of samples to maintain for the rolling average.
 */
template<typename T, size_t N>
class RollingAverage {
    static_assert(std::is_arithmetic<T>::value, "RollingAverage requires a numeric data type");

private:
    T samples[N];
    size_t sampleIndex = 0;
    T total = 0;

public:
    /**
     * @brief Constructs a new Rolling Average object.
     */
    RollingAverage() {
        memset(samples, 0, sizeof(samples));
    }

    /**
     * @brief Adds a new value to the rolling average, replacing the oldest value if necessary.
     *
     * @param value The value to add.
     * @return RollingAverage& Reference to the updated object, to allow for method chaining.
     */
    RollingAverage& add(T value) {
        total -= samples[sampleIndex];
        samples[sampleIndex] = value;
        total += value;
        sampleIndex = (sampleIndex + 1) % N;
        return *this;
    }

    /**
     * @brief Calculates and returns the current rolling average.
     *
     * @return T The current rolling average.
     */
    T average() const {
        return total / N;
    }

    /**
     * @brief Resets the rolling average, clearing all stored samples.
     */
    void reset() {
        memset(samples, 0, sizeof(samples));
        total = 0;
        sampleIndex = 0;
    }
};

#endif // ROLLING_AVERAGE_H
