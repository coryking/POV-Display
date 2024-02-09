// File: test_framebuffer.cpp
#include "FrameBuffer_t.h" // Adjust include path as necessary
#include "FrameBufferManager.h"
#include "config.h" // Your configuration file, adjust as necessary
#include "utils.h"
#include <unity.h>
#include <cstdio>
#include <cstring>

template <typename T, size_t _NUM_FRAMEBUFFERS, size_t _NUM_SEGMENTS, size_t _NUM_STEPS, size_t _NUM_LEDS_PER_SEGMENT>
FrameBufferManager_t<T, _NUM_FRAMEBUFFERS, _NUM_SEGMENTS, _NUM_STEPS, _NUM_LEDS_PER_SEGMENT> initializeFrameBufferManager()
{
    FrameBufferManager_t<T, _NUM_FRAMEBUFFERS, _NUM_SEGMENTS, _NUM_STEPS, _NUM_LEDS_PER_SEGMENT> fbm;
    // Initialize the frame buffer in a specific pattern
    for (int frame = 0; frame < _NUM_FRAMEBUFFERS; frame++)
    {
        FrameBuffer_t<T, _NUM_STEPS, _NUM_SEGMENTS, _NUM_LEDS_PER_SEGMENT> *fb = fbm.getFrameBuffer(frame);
        T *buff = fb->getBuffer();
        for (int col = 0; col < fb->getCols(); col++)
        {
            for (int row = 0; row < fb->getRows(); row++)
            {
                buff[col * fb->getRows() + row] = T({col, row, frame}); // Adjust initialization as needed
            }
        }
    }
    return fbm;
}

// Test clearing the framebuffer
void test_framebuffer_clearBuffer()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    const int rows = segments * leds_segment;
    FrameBuffer_t<uint8_t, steps, segments, leds_segment> fb; // Using uint8_t for simplicity
    fb.clearBuffer();                                         // Clear the buffer

    // Ensure the entire buffer is zeroed out
    for (unsigned int col = 0; col < steps; ++col)
    {
        for (unsigned int row = 0; row < rows; ++row)
        {
            TEST_ASSERT_EQUAL_UINT8(0, fb.getBuffer()[col * rows + row]);
        }
    }
}

// Test state transitions
void test_framebuffer_stateTransition()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t, steps, segments, leds_segment> fb;
    fb.setState(FrameBufferState::Empty);
    TEST_ASSERT_EQUAL(FrameBufferState::Empty, fb.getState());

    fb.setState(FrameBufferState::ReadyToRender);
    TEST_ASSERT_EQUAL(FrameBufferState::ReadyToRender, fb.getState());
}

// Test buffer reset functionality
void test_framebuffer_resetBuffer()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t, steps, segments, leds_segment> fb;
    fb.setState(FrameBufferState::Rendering);
    fb.resetBuffer();
    TEST_ASSERT_EQUAL(FrameBufferState::Empty, fb.getState());

    // Ensure the buffer is cleared as part of the reset
    for (unsigned int col = 0; col < 10; ++col)
    {
        for (unsigned int row = 0; row < 1; ++row)
        {
            TEST_ASSERT_EQUAL_UINT8(0, fb.getBuffer()[col * 1 + row]);
        }
    }
}

void test_framebuffer_getSegment()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 10;
    FrameBuffer_t<uint8_t, steps, segments, leds_segment> fb;
    // Initialize the frame buffer in a specific pattern
    for (int col = 0; col < fb.getCols(); col++)
    {
        for (int row = 0; row < fb.getRows(); row++)
        {
            // Assuming direct access to buffer for initialization
            fb.getBuffer()[col * fb.getRows() + row] = row; // Simple pattern: Each cell contains its row number
        }
    }

    // Testing extraction of Segment 0 from Column 0
    SegmentBuffer_t<uint8_t, leds_segment> segment = fb.getSegment(0, 0);

    // Expected values for Segment 2 based on the spec and initialization
    uint8_t expected[leds_segment];
    for (size_t i = 0; i < 10; ++i)
    {
        expected[i] = i * segments; // Matches the pattern of filling: [0,3,6] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], leds_segment);

    segment = fb.getSegment(0, 2);

    for (size_t i = 0; i < leds_segment; ++i)
    {
        expected[i] = 2 + i * segments; // Matches the pattern of filling: [2,5,9] for 9 rows and 3 segments
    }

    // Use TEST_ASSERT_EQUAL_UINT8_ARRAY to compare the expected array with the actual segment data
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, &segment.buffer[0], leds_segment);
}

void test_SegmentBuffer_Assignment_And_Access()
{
    const int leds_segment = 1;
    SegmentBuffer_t<int, leds_segment> buffer;
    buffer[0] = 42; // Use direct access to set a value

    TEST_ASSERT_EQUAL(42, buffer[0]); // Verify the value is correctly set
}

void test_StepBuffer_Assignment_And_Access()
{
    const int leds_segment = 1;
    const int segments = 3;

    StepBuffer_t<int, segments, leds_segment> stepBuffer;
    stepBuffer[0][0] = 123; // Use direct access to set a value in the first segment

    TEST_ASSERT_EQUAL(123, stepBuffer[0][0]); // Verify the value is correctly set
}

void test_StepBuffer_intoSegmentBuffer()
{
    const int leds_segment = 2;
    const int segments = 3;
    StepBuffer_t<int, segments, leds_segment> stepBuffer;
    SegmentBuffer_t<int, leds_segment> segbuff;

    segbuff[0] = 1;
    segbuff[1] = 2;

    stepBuffer[0] = segbuff;

    TEST_ASSERT_EQUAL(1, stepBuffer[0][0]);
    TEST_ASSERT_EQUAL(2, stepBuffer[0][1]);
}

struct BigJoe
{
    int c;
    int r;
    int f;
};

void test_FrameBufferManager_ProperOrder()
{
    const int steps = 4;
    const int segments = 2;
    const int leds_segment = 2;

    auto fbm = initializeFrameBufferManager<BigJoe, 3, segments, steps, leds_segment>();

    auto fb = fbm.getFrameBuffer(0);
    TEST_ASSERT_EQUAL_UINT8(0, fb->getBuffer()[0].f);
    fb = fbm.getFrameBuffer(1);
    TEST_ASSERT_EQUAL_UINT8(1, fb->getBuffer()[0].f);
    fb = fbm.getFrameBuffer(2);
    TEST_ASSERT_EQUAL_UINT8(2, fb->getBuffer()[0].f);
}

void test_FrameBufferManager_shift()
{
    const int steps = 4;
    const int segments = 2;
    const int leds_segment = 2;

    auto fbm = initializeFrameBufferManager<BigJoe, 3, segments, steps, leds_segment>();

    fbm.shiftFrames();
    auto fb = fbm.getFrameBuffer(0);
    TEST_ASSERT_EQUAL_UINT8(1, fb->getBuffer()[0].f);
    fb = fbm.getFrameBuffer(1);
    TEST_ASSERT_EQUAL_UINT8(2, fb->getBuffer()[0].f);
    fb = fbm.getFrameBuffer(2);
    TEST_ASSERT_EQUAL_UINT8(0, fb->getBuffer()[0].f);
}

void test_FrameBufferManager_generator()
{
    const int steps = 4;
    const int segments = 2;
    const int leds_segment = 2;

    auto fbm = initializeFrameBufferManager<BigJoe, 3, segments, steps, leds_segment>();

    auto fb = fbm.getGeneratorFrame();
    TEST_ASSERT_EQUAL_INT(2, fb->getBuffer()[0].f);
}

void test_FrameBufferManager_ProperFrameOrder()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 1;
    const int arm_offset = steps / segments;

    FrameBufferManager_t<int, 3, segments, steps, leds_segment> fbm;
    // Initialize the frame buffer in a specific pattern
    for (int frame = 0; frame < 3; frame++)
    {
        FrameBuffer_t<int, steps, segments, leds_segment> *fb = fbm.getFrameBuffer(frame);
        int *buff = fb->getBuffer();
        for (int col = 0; col < fb->getCols(); col++)
        {
            for (int row = 0; row < fb->getRows(); row++)
            {
                buff[col * fb->getRows() + row] = frame;
            }
        }
    }

    int expectedFrame[steps][segments] = {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}};
    char message[256]; // Buffer to hold the message
    for (int step = 0; step < steps; step++)
    {
        StepBuffer_t<int, segments, leds_segment> sb = fbm.getSegmentsForStep(step);
        for (int seg = 0; seg < segments; ++seg)
        {
            snprintf(message, sizeof(message), "Step: %d, Segment: %d", step, seg);

            TEST_ASSERT_EQUAL_INT_MESSAGE(expectedFrame[step][seg], sb[seg][0], message);
        }
    }
}

void test_FrameBufferManager_PrintItOut()
{
    const int steps = 3;
    const int segments = 3;
    const int leds_segment = 3;

    auto fbm = initializeFrameBufferManager<BigJoe, 3, segments, steps, leds_segment>();

    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        StepBuffer_t<BigJoe, segments, leds_segment> sb = fbm.getSegmentsForStep(step);

        for (int seg = 0; seg < segments; ++seg)
        {
            snprintf(message, sizeof(message), "Step: %d, Segment %d: |", step, seg);
            for (int led = 0; led < leds_segment; ++led)
            {
                // Assuming each LED value can be represented in at most 10 characters
                char ledValue[256];
                snprintf(ledValue, sizeof(ledValue), " f: %d, c: %d, r: %d |", sb[seg][led].f, sb[seg][led].c, sb[seg][led].r);
                // Make sure we don't overflow the message buffer
                if (strlen(message) + strlen(ledValue) < sizeof(message) - 1)
                {
                    strcat(message, ledValue);
                }
                else
                {
                    // If the message is full, print it and start a new message
                    TEST_MESSAGE(message);
                    snprintf(message, sizeof(message), "Step: %d, Segment %d (cont) f: %d, c: %d, r: %d ", step, seg, sb[seg][led].f, sb[seg][led].c, sb[seg][led].r);
                }
            }
            // Print the remaining message for the segment
            TEST_MESSAGE(message);
        }
        TEST_MESSAGE("-------");
    }
    /*  for (int seg = 0; seg < segments; seg++) {
         int exp[leds_segment] = {1,2,3,4};
         TEST_ASSERT_EQUAL_UINT8_ARRAY(exp, &sb[seg][0],leds_segment);
     } */
}

void test_Utils_arm_position()
{
    int steps = 3;
    int arms = 3;
    int positions[3][3] = {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}};

    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        for (int arm = 0; arm < arms; arm++)
        {
            snprintf(message, sizeof(message), "Step: %d, Arm: %d", step, arm);

            int position = compute_postition_of_arm(step, arm, 1);
            TEST_ASSERT_EQUAL_INT_MESSAGE(positions[step][arm], position, message);
        }
    }
}

void test_Utils_compute_arm_column()
{
    const int steps = 3;
    const int arms = 3;
    int arm_positions[steps][arms] = {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}};
    int expected_outputs[steps][arms] = {{0, 2, 1}, {1, 0, 2}, {2, 1, 0}};

    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        for (int arm = 0; arm < arms; arm++)
        {
            int arm_position = arm_positions[step][arm];
            snprintf(message, sizeof(message), "Step: %d, Arm: %d, Arm Pos: %d", step, arm, arm_position);

            int column_number = compute_arm_column(arm_position, steps);
            int expected_output = expected_outputs[step][arm];
            TEST_ASSERT_EQUAL_INT_MESSAGE(expected_output, column_number, message);
        }
    }
}

void test_Utils_compute_frame_offset()
{
    const int steps = 3;
    const int arms = 3;
    int arm_positions[steps][arms] = {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}};
    int expected_outputs[steps][arms] = {{0, -1, -1}, {0, 0, -1}, {0, 0, 0}};

    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        for (int arm = 0; arm < arms; arm++)
        {
            int arm_position = arm_positions[step][arm];

            snprintf(message, sizeof(message), "Step: %d, Arm: %d, Input: %d", step, arm, arm_position);

            int column_number = compute_frame_offset(arm_position, steps);
            int expected_output = expected_outputs[step][arm];
            TEST_ASSERT_EQUAL_INT_MESSAGE(expected_output, column_number, message);
        }
    }
}

void test_Utils_compute_frame_number()
{
    const int steps = 3;
    const int arms = 3;
    int arm_positions[steps][arms] = {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}};
    int expected_outputs[steps][arms] = {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}};

    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        for (int arm = 0; arm < arms; arm++)
        {
            int arm_position = arm_positions[step][arm];

            snprintf(message, sizeof(message), "Step: %d, Arm: %d, Input: %d", step, arm, arm_position);

            int column_number = compute_frame_number(arm_position, steps);
            int expected_output = expected_outputs[step][arm];
            TEST_ASSERT_EQUAL_INT_MESSAGE(expected_output, column_number, message);
        }
    }
}

void test_get_column_frame()
{
    const int steps = 3;
    const int arms = 3;

    //int arm_positions[steps][arms] = {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}};
    ColumnFrame expected_outputs[steps][arms] = {{
                                                     {0, 1},
                                                     {2, 0},
                                                     {1, 0},
                                                 },
                                                 {
                                                     {1, 1},
                                                     {0, 1},
                                                     {2, 0},
                                                 },
                                                 {
                                                     {2, 1},
                                                     {1, 1},
                                                     {0, 1},
                                                 }};
    char message[256]; // Buffer to hold the message

    for (int step = 0; step < steps; step++)
    {
        for (int arm = 0; arm < arms; arm++)
        {

            ColumnFrame cf = get_column_frame(arm, arms, step, steps);
            ColumnFrame expected_output = expected_outputs[step][arm];
            snprintf(message, sizeof(message), "Column, Step: %d, Arm: %d", step, arm);

            TEST_ASSERT_EQUAL_MESSAGE(cf.column, expected_output.column, message);
            snprintf(message, sizeof(message), "Frame, Step: %d, Arm: %d", step, arm);
            TEST_ASSERT_EQUAL_MESSAGE(cf.frame, expected_output.frame, message);
            //TEST_ASSERT_EQUAL_INT_MESSAGE(expected_output, column_number, message);
        }
    }
}
void test_mod_positive_dividend(void)
{
    TEST_ASSERT_EQUAL_INT(1, mod(4, 3));
    TEST_ASSERT_EQUAL_INT(0, mod(3, 3));
}

void test_mod_negative_dividend(void)
{
    TEST_ASSERT_EQUAL_INT(2, mod(-1, 3));
    TEST_ASSERT_EQUAL_INT(1, mod(-5, 3));
}

void test_mod_with_zero_dividend(void)
{
    TEST_ASSERT_EQUAL_INT(0, mod(0, 3));
}

void setUp(void)
{
    // Set up any required structures here
}

void tearDown(void)
{
    // Clean up any resources here
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_framebuffer_clearBuffer);
    RUN_TEST(test_framebuffer_stateTransition);
    RUN_TEST(test_framebuffer_resetBuffer);
    RUN_TEST(test_SegmentBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_intoSegmentBuffer);
    RUN_TEST(test_framebuffer_getSegment);
    RUN_TEST(test_FrameBufferManager_ProperOrder);
    RUN_TEST(test_FrameBufferManager_shift);
    RUN_TEST(test_FrameBufferManager_PrintItOut);
    RUN_TEST(test_FrameBufferManager_ProperFrameOrder);
    RUN_TEST(test_Utils_arm_position);
    RUN_TEST(test_Utils_compute_arm_column);
    RUN_TEST(test_Utils_compute_frame_offset);
    RUN_TEST(test_Utils_compute_frame_number);
    RUN_TEST(test_mod_positive_dividend);
    RUN_TEST(test_mod_negative_dividend);
    RUN_TEST(test_mod_with_zero_dividend);
    RUN_TEST(test_get_column_frame);
    UNITY_END();
}
