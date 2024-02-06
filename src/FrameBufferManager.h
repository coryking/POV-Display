#ifndef __FRAMEBUFFERMANAGER_H__
#define __FRAMEBUFFERMANAGER_H__

#include <FreeRTOS.h>
#include <queue.h>
#include "FrameBuffer.h"
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
        if (_needsFrameShift)
        {
            // clear the old one out...
            getFrameBuffer(0)->resetBuffer();
            buffer_offset = (buffer_offset + 1) % _FRAMEBUFFERS; // Wrap around to ensure it stays within bounds
            _needsFrameShift = false;
        }
    }

    StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> getSegmentsForStep(step_t step)
    {
        uint frame; //[_NUM_SEGMENTS];              // from frame
        uint lowest_frame = 1000;                // get the lowest frame needed... if it isn't 0 than we can shift the buffers around
        StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> output;
        for (int i = 0; i < _NUM_SEGMENTS; i++)
        {
            column_num_t column = step - _ARM_OFFSET * i;
            frame = 1 - static_cast<int>(column / _NUM_STEPS);
            lowest_frame = std::min(frame, lowest_frame);
            //output[i] = getFrameBuffer(frame).assertInState(FrameBufferState::ReadyToRender)->getSegment(column, i);
            output[i] = getFrameBuffer(frame)->getSegment(column % _NUM_STEPS, i); // Ensure column is within bounds
        }
        // if nothing is using frame 0 (the oldest) then we need to shift frames
        if (lowest_frame > 0)
            _needsFrameShift = true;

        return output;
    }

    FrameBuffer_t<T>* getFrameBuffer(uint frame)
    {
        uint actualIndex = (frame + buffer_offset) % _FRAMEBUFFERS;
        return &frameBuffers[actualIndex];
    }

    FrameBuffer_t<T>* getGeneratorFrame()
    {
        return getFrameBuffer(_FRAMEBUFFERS - 1);
    }

private:
    FrameBuffer_t<T> frameBuffers[_FRAMEBUFFERS];
    uint buffer_offset = 0;
    bool _needsFrameShift = false;
};


#endif // __FRAMEBUFFERMANAGER_H__