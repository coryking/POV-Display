#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include "types.h"

// this is to mimic how python or excel handle the modulo operator
int mod(int a, int b) {

    int result = a % b;
    if (result < 0) {
        result += b;
    }
    return result;
}

// this is to mimic how python or excel handle division
int floorDiv(int dividend, int divisor) {
    assert(divisor > 0); // Ensure divisor is positive.
    if (dividend >= 0) {
        return dividend / divisor;
    } else {
        // Adjust for negative dividends to achieve floor division.
        return (dividend - divisor + 1) / divisor;
    }
}


struct ColumnFrame {
    column_num_t column;
    int frame;
};

int compute_postition_of_arm(step_t step, uint8_t arm, uint16_t arm_offset) {
    return step - (arm_offset * arm);
}
int compute_arm_column(int arm_position, uint8_t total_steps) {
    
    return mod(arm_position, static_cast<int>(total_steps));
}
int compute_frame_offset(int arm_position, step_t total_steps) {
    return floorDiv(arm_position, total_steps);
}

int compute_frame_number(int arm_position, step_t total_steps) {
    return 1-std::abs(compute_frame_offset(arm_position, total_steps));
}

ColumnFrame get_column_frame(uint8_t segment, uint8_t segments, step_t step, step_t total_steps)
{
    int arm_position = compute_postition_of_arm(step, segment, segments/total_steps);
    int frame = compute_frame_number(arm_position, total_steps);
    column_num_t column = compute_arm_column(arm_position, total_steps);

    return ColumnFrame{.column = column, .frame = frame};

}

#endif // __UTILS_H__