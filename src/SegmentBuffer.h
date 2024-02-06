#ifndef __SEGMENTBUFFER_H__
#define __SEGMENTBUFFER_H__


#include <stddef.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include "config.h"

template <typename T, size_t _NUM_LEDS_PER_SEGMENT>
class SegmentBuffer_t {
public:
    std::vector<T> buffer;

    SegmentBuffer_t() : buffer(_NUM_LEDS_PER_SEGMENT) {}

    T& operator[](size_t index) {
        assert(index < buffer.size());
        return buffer[index];
    }

    const T& operator[](size_t index) const {
        assert(index < buffer.size());
        return buffer[index];
    }

    size_t size() const {
        return buffer.size();
    }
};


template <typename T, size_t _NUM_SEGMENTS, size_t _NUM_LEDS_PER_SEGMENT>
class StepBuffer_t {
public:
    std::vector<SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>> segments;

    StepBuffer_t() : segments(_NUM_SEGMENTS) {
        for (size_t i = 0; i < _NUM_SEGMENTS; ++i) {
            segments[i] = SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>();
        }
    }

    StepBuffer_t(const T (&initialValues)[_NUM_SEGMENTS][_NUM_LEDS_PER_SEGMENT])
        : segments(_NUM_SEGMENTS) {
        for (size_t i = 0; i < _NUM_SEGMENTS; ++i) {
            std::vector<T> initVec(initialValues[i], initialValues[i] + _NUM_LEDS_PER_SEGMENT);
            segments[i] = SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>(initVec);
        }
    }

    SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>& operator[](size_t index) {
        if (index >= segments.size()) throw std::out_of_range("Index out of range");
        return segments[index];
    }

    const SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>& operator[](size_t index) const {
        if (index >= segments.size()) throw std::out_of_range("Index out of range");
        return segments[index];
    }

    size_t size() const {
        return segments.size();
    }
};



#endif // __SEGMENTBUFFER_H__