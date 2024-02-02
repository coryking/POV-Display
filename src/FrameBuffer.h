#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <FastLED.h>
#include <cassert>
#include <cstring> // For memset
#include "config.h"


enum class FrameBufferStatus {
    Empty,
    QueuedForGeneration,
    Generating,
    ReadyToRender,
    Rendering,
    RenderingComplete
};

template<typename T, unsigned int COLS, unsigned int ROWS, unsigned int SEGMENTS>
class FrameBuffer_t {
public:
    FrameBuffer_t() : _status(FrameBufferStatus::Empty) {
        static_assert(COLS % SEGMENTS == 0, "Columns must be divisible by segments.");
        static_assert(sizeof(T) == 1 || (sizeof(T) & (sizeof(T) - 1)) == 0, "T must be 1 byte or power of 2 bytes size for memset to be safe.");
    }

    T* getBuffer() {
        return &buffer[0][0];
    }

    void getSegment(unsigned int column, unsigned int segment, T output[ROWS / SEGMENTS]) {
        assert(segment < SEGMENTS);
        for (unsigned int virtcol = segment, segcol = 0; segcol < _segmentSize; virtcol += SEGMENTS, ++segcol) {
                output[segcol] = buffer[column][virtcol];
        }
    }

    unsigned int getCols() const {
        return COLS;
    }

    unsigned int getRows() const {
        return ROWS;
    }

    FrameBufferStatus getStatus() {
        return _status;
    }

    /**
     * @brief Set the status of this framebuffer.
     * 
     * Note this does not enforce any kind of workflow. You can
     * set this to anything no matter what state it is in right now.
     * 
     * @param newStatus -- what the new status should be
     * @return FrameBufferStatus -- returns the old status. 
     */
    FrameBufferStatus setStatus(FrameBufferStatus newStatus) {
        FrameBufferStatus oldStatus = _status;
        _status = newStatus;
        return oldStatus;
    }

    bool clearBuffer() {
        if(_status == FrameBufferStatus::ReadyToRender || _status == FrameBufferStatus::Rendering || _status== FrameBufferStatus::Generating)
            return false;
        // Use memset to clear the buffer, assuming T can be safely zeroed out.
        memset(buffer, 0, sizeof(buffer));
        _status = FrameBufferStatus::Empty;

        return true;
    }

private:
    T buffer[COLS][ROWS];
    const uint _segmentSize = COLS / SEGMENTS;
    FrameBufferStatus _status;
};

typedef FrameBuffer_t<CRGB, DEGREES_PER_COLUMN, NUM_ROWS, NUM_SEGMENTS> FrameBuffer;


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

#endif // FRAMEBUFFER_H
