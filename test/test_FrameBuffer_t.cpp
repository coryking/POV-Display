// File: test_framebuffer.cpp
#include "FrameBuffer_t.h"  // Adjust include path as necessary
#include "config.h"         // Your configuration file, adjust as necessary
#include <unity.h>

// Test clearing the framebuffer
void test_framebuffer_clearBuffer() {
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    const int rows = segments * leds_segment;
    FrameBuffer_t<uint8_t,steps,segments,leds_segment> fb;  // Using uint8_t for simplicity
    fb.clearBuffer();           // Clear the buffer

    // Ensure the entire buffer is zeroed out
    for (unsigned int col = 0; col < steps; ++col) {
        for (unsigned int row = 0; row < rows; ++row) {
            TEST_ASSERT_EQUAL_UINT8(0, fb.getBuffer()[col * rows + row]);
        }
    }
}

// Test state transitions
void test_framebuffer_stateTransition() {
        const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t,steps,segments,leds_segment> fb;    fb.setState(FrameBufferState::Empty);
    TEST_ASSERT_EQUAL(FrameBufferState::Empty, fb.getState());

    fb.setState(FrameBufferState::ReadyToRender);
    TEST_ASSERT_EQUAL(FrameBufferState::ReadyToRender, fb.getState());
}

// Test buffer reset functionality
void test_framebuffer_resetBuffer() {
        const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t,steps,segments,leds_segment> fb;
    fb.setState(FrameBufferState::Rendering);
    fb.resetBuffer();
    TEST_ASSERT_EQUAL(FrameBufferState::Empty, fb.getState());

    // Ensure the buffer is cleared as part of the reset
    for (unsigned int col = 0; col < 10; ++col) {
        for (unsigned int row = 0; row < 1; ++row) {
            TEST_ASSERT_EQUAL_UINT8(0, fb.getBuffer()[col * 1 + row]);
        }
    }
}

void test_framebuffer_getSegment() {
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t,steps,segments,leds_segment> fb;
    // Initialize the frame buffer in a specific pattern
    for (int col = 0; col < fb.getCols(); col++) {
        for (int row = 0; row < fb.getRows(); row++) {
            // Assuming direct access to buffer for initialization
            fb.getBuffer()[col * fb.getRows() + row] = row; // Simple pattern: Each cell contains its row number
        }
    }

    // Testing extraction of Segment 0 from Column 0
    SegmentBuffer_t<uint8_t,leds_segment> segment = fb.getSegment(0, 0);

    // Expected values for Segment 2 based on the spec and initialization
    uint8_t expected[leds_segment];
    for (size_t i = 0; i < 10; ++i) {
        expected[i] = i * segments; // Matches the pattern of filling: [0,3,6] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], leds_segment);

    segment = fb.getSegment(0, 2);

    for (size_t i = 0; i < leds_segment; ++i) {
        expected[i] = 2+ i * segments; // Matches the pattern of filling: [2,5,9] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], leds_segment);
}



void test_SegmentBuffer_Assignment_And_Access() {
    const int leds_segment = 1;
    SegmentBuffer_t<int,leds_segment> buffer;
    buffer[0] = 42; // Use direct access to set a value
    
    TEST_ASSERT_EQUAL(42, buffer[0]); // Verify the value is correctly set
}


void test_StepBuffer_Assignment_And_Access() {
    const int leds_segment = 1;
    const int segments = 3;


    StepBuffer_t<int,segments,leds_segment> stepBuffer;
    stepBuffer[0][0] = 123; // Use direct access to set a value in the first segment
    
    TEST_ASSERT_EQUAL(123, stepBuffer[0][0]); // Verify the value is correctly set
}


void test_StepBuffer_intoSegmentBuffer() {
    const int leds_segment = 2;
    const int segments = 3;
    StepBuffer_t<int,segments,leds_segment> stepBuffer;
    SegmentBuffer_t<int,leds_segment> segbuff;

    segbuff[0] = 1;
    segbuff[1] = 2;

    stepBuffer[0] = segbuff;

    TEST_ASSERT_EQUAL(1, stepBuffer[0][0]);
    TEST_ASSERT_EQUAL(2, stepBuffer[0][1]);
}


void setUp(void) {
    // Set up any required structures here
}

void tearDown(void) {
    // Clean up any resources here
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_framebuffer_clearBuffer);
    RUN_TEST(test_framebuffer_stateTransition);
    RUN_TEST(test_framebuffer_resetBuffer);
     RUN_TEST(test_SegmentBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_intoSegmentBuffer);
    RUN_TEST(test_framebuffer_getSegment);
    UNITY_END();
}
