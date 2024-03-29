#ifndef __FRAMEBUFFERMANAGER_H__
#define __FRAMEBUFFERMANAGER_H__

#include "FrameBuffer_t.h"
#include "SegmentBuffer.h"
#include "types.h"
#include "utils.h"

#define NUM_FRAMEBUFFERS 3
template <typename T, size_t _FRAMEBUFFERS, size_t _NUM_SEGMENTS, size_t _NUM_STEPS, size_t _NUM_LEDS_PER_SEGMENT>
class FrameBufferManager_t
{
public:
    FrameBufferManager_t() : arm_offset(_NUM_STEPS/_NUM_SEGMENTS) {}


    void shiftFrames()
    {
        // clear the old one out...
        getFrameBuffer(0)->resetBuffer();
        buffer_offset = (buffer_offset + 1) % _FRAMEBUFFERS; // Wrap around to ensure it stays within bounds
    }

    StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> getSegmentsForStep(step_t step)
    {
        StepBuffer_t<T, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> output;

        for (int seg = 0; seg < _NUM_SEGMENTS; seg++)
        {
            ColumnFrame cf = get_column_frame(seg, _NUM_SEGMENTS, step, _NUM_STEPS);

            output[seg] = getFrameBuffer(cf.frame)->getSegment(cf.column, seg);
        }
      

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
    const int arm_offset;
};


#endif // __FRAMEBUFFERMANAGER_H__