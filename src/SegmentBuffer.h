#ifndef __SEGMENTBUFFER_H__
#define __SEGMENTBUFFER_H__


#include <stddef.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include "config.h"
template <typename T, size_t _NUM_LEDS_PER_SEGMENT> class SegmentBuffer_t
{
  public:
    T buffer[_NUM_LEDS_PER_SEGMENT];

    T &operator[](size_t index)
    {
        assert(index < _NUM_LEDS_PER_SEGMENT);
        return buffer[index];
    }

    const T &operator[](size_t index) const
    {
        assert(index < _NUM_LEDS_PER_SEGMENT);
        return buffer[index];
    }

    size_t size() const
    {
        return _NUM_LEDS_PER_SEGMENT;
    }
};
template <typename T, size_t _NUM_SEGMENTS, size_t _NUM_LEDS_PER_SEGMENT> class StepBuffer_t
{
  public:
    SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT> segments[_NUM_SEGMENTS];

    StepBuffer_t()
    {
        for (size_t i = 0; i < _NUM_SEGMENTS; ++i)
        {
            segments[i] = SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT>();
        }
    }

    SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT> &operator[](size_t index)
    {
        if (index >= _NUM_SEGMENTS)
            throw std::out_of_range("Index out of range");
        return segments[index];
    }

    const SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT> &operator[](size_t index) const
    {
        if (index >= _NUM_SEGMENTS)
            throw std::out_of_range("Index out of range");
        return segments[index];
    }

    size_t size() const
    {
        return _NUM_SEGMENTS;
    }
};

#endif // __SEGMENTBUFFER_H__