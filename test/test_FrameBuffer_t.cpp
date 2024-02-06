// File: test_framebuffer.cpp
#include "FrameBuffer_t.h"  // Adjust include path as necessary
#include "config.h"         // Your configuration file, adjust as necessary
#include <unity.h>

// Test clearing the framebuffer
void test_framebuffer_clearBuffer() {
    FrameBuffer_t<uint8_t,1,3,10> fb;  // Using uint8_t for simplicity
    fb.clearBuffer();           // Clear the buffer

    // Ensure the entire buffer is zeroed out
    for (unsigned int col = 0; col < 1; ++col) {
        for (unsigned int row = 0; row < NUM_ROWS; ++row) {
            TEST_ASSERT_EQUAL_UINT8(0, fb.getBuffer()[col * NUM_ROWS + row]);
        }
    }
}

// Test state transitions
void test_framebuffer_stateTransition() {
    FrameBuffer_t<uint8_t,1,1,1> fb;
    fb.setState(FrameBufferState::Empty);
    TEST_ASSERT_EQUAL(FrameBufferState::Empty, fb.getState());

    fb.setState(FrameBufferState::ReadyToRender);
    TEST_ASSERT_EQUAL(FrameBufferState::ReadyToRender, fb.getState());
}

// Test buffer reset functionality
void test_framebuffer_resetBuffer() {
    FrameBuffer_t<uint8_t,10,1,1> fb;
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
    FrameBuffer_t<uint8_t,2,3,10> fb;
    // Initialize the frame buffer in a specific pattern
    for (int col = 0; col < fb.getCols(); col++) {
        for (int row = 0; row < fb.getRows(); row++) {
            // Assuming direct access to buffer for initialization
            fb.getBuffer()[col * fb.getRows() + row] = row; // Simple pattern: Each cell contains its row number
        }
    }

    // Testing extraction of Segment 0 from Column 0
    SegmentBuffer_t<uint8_t,10> segment = fb.getSegment(0, 0);

    // Expected values for Segment 2 based on the spec and initialization
    uint8_t expected[10];
    for (size_t i = 0; i < 10; ++i) {
        expected[i] = i * NUM_SEGMENTS; // Matches the pattern of filling: [0,3,6] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], NUM_LEDS_PER_SEGMENT);

    segment = fb.getSegment(0, 2);

    for (size_t i = 0; i < NUM_LEDS_PER_SEGMENT; ++i) {
        expected[i] = 2+ i * NUM_SEGMENTS; // Matches the pattern of filling: [2,5,9] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], NUM_LEDS_PER_SEGMENT);
}



void test_SegmentBuffer_Assignment_And_Access() {
    SegmentBuffer_t<int> buffer;
    buffer[0] = 42; // Use direct access to set a value
    
    TEST_ASSERT_EQUAL(42, buffer[0]); // Verify the value is correctly set
}


void test_StepBuffer_Assignment_And_Access() {
    StepBuffer_t<int> stepBuffer;
    stepBuffer[0][0] = 123; // Use direct access to set a value in the first segment
    
    TEST_ASSERT_EQUAL(123, stepBuffer[0][0]); // Verify the value is correctly set
}


void test_StepBuffer_intoSegmentBuffer() {
    StepBuffer_t<int> stepBuffer;
    SegmentBuffer_t<int> segbuff;

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
