#ifndef __FRAMEBUFFERMANAGER_H__
#define __FRAMEBUFFERMANAGER_H__

#include "FrameBuffer_t.h"
#include "SegmentBuffer.h"
#include "types.h"

#define NUM_FRAMEBUFFERS 3
template <typename T, size_t _FRAMEBUFFERS, size_t _NUM_SEGMENTS, int _ARM_OFFSET, size_t _NUM_STEPS, size_t _NUM_LEDS_PER_SEGMENT>
class FrameBufferManager_t
{
public:
    FrameBufferManager_t() {}

    bool needsFrameShift()
    {
        return _needsFrameShift;
    }

    void shiftFrames()
    {
        //if (_needsFrameShift)
        //{
            // clear the old one out...
            getFrameBuffer(0)->resetBuffer();
            buffer_offset = (buffer_offset + 1) % _FRAMEBUFFERS; // Wrap around to ensure it stays within bounds
            _needsFrameShift = false;
        //}
    }

    StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> getSegmentsForStep(step_t step)
    {
        uint8_t lowest_frame = 255;                // get the lowest frame needed... if it isn't 0 than we can shift the buffers around
        StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> output;
        for (int seg = 0; seg < _NUM_SEGMENTS; seg++)
        {
            column_num_t column = abs(step - _ARM_OFFSET * seg);
            uint8_t frame = 1 - static_cast<int>(column / _NUM_STEPS);
            lowest_frame = std::min(frame, lowest_frame);
            //output[seg] = getFrameBuffer(frame).assertInState(FrameBufferState::ReadyToRender)->getSegment(column, i);
            output[seg] = getFrameBuffer(frame)->getSegment(column % _NUM_STEPS, seg); // Ensure column is within bounds
        }
        // if nothing is using frame 0 (the oldest) then we need to shift frames
        if (lowest_frame > 0)
            _needsFrameShift = true;

        return output;
    }

    FrameBuffer_t<T, _NUM_STEPS, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT>* getFrameBuffer(uint8_t frame)
    {
        uint8_t actualIndex = (frame + buffer_offset) % _FRAMEBUFFERS;
        return &frameBuffers[actualIndex];
    }

    FrameBuffer_t<T, _NUM_STEPS, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT>* getGeneratorFrame()
    {
        return getFrameBuffer(_FRAMEBUFFERS - 1);
    }

private:
    FrameBuffer_t<T, _NUM_STEPS, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> frameBuffers[_FRAMEBUFFERS];
    uint8_t buffer_offset = 0;
    bool _needsFrameShift = false;
};


#endif // __FRAMEBUFFERMANAGER_H__