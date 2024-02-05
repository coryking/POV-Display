#ifndef __FRAMEBUFFERMANAGER_H__
#define __FRAMEBUFFERMANAGER_H__

#include <FreeRTOS.h>
#include <queue.h>
#include "FrameBuffer.h"
#include "SegmentBuffer.h"
#include "types.h"

#define NUM_FRAMEBUFFERS 3

template <typename T>
class FrameBufferManager_t
{
public:
    FrameBufferManager_t(){

    };
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
            buffer_offset = (buffer_offset + 1) % NUM_FRAMEBUFFERS; // Wrap around to ensure it stays within bounds
            _needsFrameShift = false;
        }
    };

    StepBuffer_t<T> getSegmentsForStep(step_t step)
    {
        uint frame; //[NUM_SEGMENTS];              // from frame
        uint lowest_frame = 1000;              // get the lowest frame needed... if it isn't 0 than we can shift the buffers around
        StepBuffer_t<T> output;
        for (int i = 0; i < NUM_SEGMENTS; i++)
        {
            column_num_t column = step - ARM_OFFSET * i;
            uint frame = 1 - static_cast<int>(column / NUM_STEPS);
            lowest_frame = min(frame, lowest_frame);
            //output[i] = getFrameBuffer(frame).assertInState(FrameBufferState::ReadyToRender)->getSegment(column, i);
            output[i] = getFrameBuffer(frame)->getSegment(column, i);
        }
        // if nothing is using frame 0 (the oldest) then we need to shift frames
        if (lowest_frame > 0)
            _needsFrameShift = true;

        return output;
    };

    FrameBuffer_t<T>* getFrameBuffer(uint frame)
    {
        uint actualIndex = (frame + buffer_offset) % NUM_FRAMEBUFFERS;
        return &frameBuffers[actualIndex];
    };

    FrameBuffer_t<T>* getGeneratorFrame()
    {
        return getFrameBuffer(NUM_FRAMEBUFFERS - 1);
    }

private:
    FrameBuffer_t<T> frameBuffers[NUM_FRAMEBUFFERS];
    uint buffer_offset = 0;
    bool _needsFrameShift = false;
};

#endif // __FRAMEBUFFERMANAGER_H__