#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <FastLED.h>
#include <cassert>
#include <cstring> // For memset
#include "config.h"
#include "SegmentBuffer.h"


enum class FrameBufferState {
    Empty,
    QueuedForGeneration,
    Generating,
    ReadyToRender,
    Rendering,
    RenderingComplete
};

template<typename T>
class FrameBuffer_t {
public:
    FrameBuffer_t() : _state(FrameBufferState::Empty) {
        static_assert(NUM_STEPS % NUM_SEGMENTS == 0, "Columns must be divisible by segments.");
        //static_assert(sizeof(T) == 1 || (sizeof(T) & (sizeof(T) - 1)) == 0, "T must be 1 byte or power of 2 bytes size for memset to be safe.");
    }

    T* getBuffer() {
        return &buffer[0][0];
    }
    SegmentBuffer_t<T> getSegment(unsigned int column, unsigned int segment) {
        assert(segment < NUM_SEGMENTS);
        
        SegmentBuffer_t<T> output;
        
        for (unsigned int virtcol = segment, segcol = 0; segcol < NUM_LEDS_PER_SEGMENT; virtcol += NUM_SEGMENTS, ++segcol) {
            output[segcol] = buffer[column][virtcol];
        }

        return output;
    }

    FrameBuffer_t<T>* assertInState(FrameBufferState state) {
        assert(_state == state);
        return this;
    }

    unsigned int getCols() const {
        return NUM_STEPS;
    }

    unsigned int getRows() const {
        return NUM_ROWS;
    }

    FrameBufferState getState() {
        return _state;
    }

    /**
     * @brief Set the status of this framebuffer.
     * 
     * Note this does not enforce any kind of workflow. You can
     * set this to anything no matter what state it is in right now.
     * 
     * @param newState -- what the new status should be
     * @return FrameBufferState -- returns the old status. 
     */
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
        //if(_state == FrameBufferState::ReadyToRender || _state == FrameBufferState::Rendering || _state== FrameBufferState::Generating)
        //    return false;
        // Use memset to clear the buffer, assuming T can be safely zeroed out.
        memset(buffer, 0, sizeof(buffer));
        _state = FrameBufferState::Empty;

        return true;
    }

private:
    T buffer[NUM_STEPS][NUM_ROWS];
    const uint _segmentSize = NUM_LEDS_PER_SEGMENT;
    FrameBufferState _state;
};

typedef FrameBuffer_t<CRGB> FrameBuffer;


typedef struct
{
    /**
     * @brief The time this frame will be rendered
     *
     */
    timestamp_t frame_time;
    /**
     * @brief Pointer to the framebuffer that we will be spewing our crap into
     *
     */
    FrameBuffer *framebuffer;
} generatorParams;

#endif // __FRAMEBUFFER_H__