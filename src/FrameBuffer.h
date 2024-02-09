#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__


#include "FrameBuffer_t.h"
#include <FastLED.h>


typedef FrameBuffer_t<CRGB,NUM_STEPS,NUM_SEGMENTS, NUM_LEDS_PER_SEGMENT> FrameBuffer;


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