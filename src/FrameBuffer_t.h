#ifndef __FRAMEBUFFER_T_H__
#define __FRAMEBUFFER_T_H__

#include <cassert>
#include <cstring> // For memset
#include "config.h"
#include "SegmentBuffer.h"

enum class FrameBufferState
{
    Empty,
    QueuedForGeneration,
    Generating,
    ReadyToRender,
    Rendering,
    RenderingComplete
};

template <typename T, size_t _NUM_STEPS, size_t _NUM_SEGMENTS, size_t _NUM_LEDS_PER_SEGMENT>
class FrameBuffer_t {
public:
    FrameBuffer_t() : _state(FrameBufferState::Empty) {
        static_assert(_NUM_STEPS % _NUM_SEGMENTS == 0, "Columns must be divisible by segments.");
    }

    T* getBuffer() {
        return &buffer[0][0];
    }

    SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT> getSegment(unsigned int columnIndex, unsigned int segmentNumber) {
        assert(segmentNumber < _NUM_SEGMENTS);

        SegmentBuffer_t<T, _NUM_LEDS_PER_SEGMENT> output;

        for (unsigned int rowIndex = segmentNumber, segmentIndex = 0; segmentIndex < _NUM_LEDS_PER_SEGMENT; rowIndex += _NUM_SEGMENTS, ++segmentIndex) {
            output[segmentIndex] = buffer[columnIndex][rowIndex];
        }

        return output;
    }

    FrameBuffer_t<T, _NUM_STEPS, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT>* assertInState(FrameBufferState state) {
        assert(_state == state);
        return this;
    }

    unsigned int getCols() const {
        return _NUM_STEPS;
    }

    unsigned int getRows() const {
        return _NUM_LEDS_PER_SEGMENT * _NUM_SEGMENTS;
    }

    FrameBufferState getState() {
        return _state;
    }

    FrameBufferState setState(FrameBufferState newState) {
        FrameBufferState oldState = _state;
        _state = newState;
        return oldState;
    }

    bool resetBuffer() {
        _state = FrameBufferState::RenderingComplete;
        return clearBuffer();
    }

    bool clearBuffer() {
        memset(buffer, 0, sizeof(buffer));
        _state = FrameBufferState::Empty;

        return true;
    }

private:
    T buffer[_NUM_STEPS][_NUM_LEDS_PER_SEGMENT * _NUM_SEGMENTS];
    FrameBufferState _state;
};

#endif // __FRAMEBUFFER_T_H__