#include <SegmentBuffer.h>
#include <unity.h>

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
    RUN_TEST(test_SegmentBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_Assignment_And_Access);
    RUN_TEST(test_StepBuffer_intoSegmentBuffer);
    UNITY_END();
}
