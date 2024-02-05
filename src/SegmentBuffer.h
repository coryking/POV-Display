#ifndef __SEGMENTBUFFER_H__
#define __SEGMENTBUFFER_H__


#include <stddef.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include "config.h"

template <typename T>
class SegmentBuffer_t {
public:
    std::vector<T> buffer;

    SegmentBuffer_t() : buffer(NUM_LEDS_PER_SEGMENT) {}

    // Direct access to elements (read-write)
    T& operator[](size_t index) {
        assert(index < buffer.size());
        return buffer[index];
    }

    // Const access to elements (read-only)
    const T& operator[](size_t index) const {
        assert(index < buffer.size());
        return buffer[index];
    }

    size_t size() const {
        return buffer.size();
    }
};


template <typename T>
class StepBuffer_t {
public:
    std::vector<SegmentBuffer_t<T>> segments;

    StepBuffer_t() : segments(NUM_SEGMENTS) {
        for (size_t i = 0; i < NUM_SEGMENTS; ++i) {
            segments[i] = SegmentBuffer_t<T>();
        }
    }

    StepBuffer_t(const T (&initialValues)[NUM_SEGMENTS][NUM_LEDS_PER_SEGMENT]) : segments(NUM_SEGMENTS) {
        for (size_t i = 0; i < NUM_SEGMENTS; ++i) {
            // Assuming SegmentBuffer_t constructor takes a std::vector or can be modified to do so
            std::vector<T> initVec(initialValues[i], initialValues[i] + NUM_LEDS_PER_SEGMENT);
            segments[i] = SegmentBuffer_t<T>(initVec);
        }
    }

    // Provide read-write access to the segments
    SegmentBuffer_t<T>& operator[](size_t index) {
        if (index >= segments.size()) throw std::out_of_range("Index out of range");
        return segments[index];
    }

    // Provide read-only access to the segments (overload for const contexts)
    const SegmentBuffer_t<T>& operator[](size_t index) const {
        if (index >= segments.size()) throw std::out_of_range("Index out of range");
        return segments[index];
    }

    size_t size() const {
        return segments.size();
    }
};



#endif // __SEGMENTBUFFER_H__