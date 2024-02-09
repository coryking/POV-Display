#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include "types.h"

/**
 * @struct ColumnFrame
 * @brief Holds information about a column and its corresponding frame.
 *
 * @var ColumnFrame::column
 * Column number.
 * @var ColumnFrame::frame
 * Frame number.
 */
struct ColumnFrame
{
    column_num_t column;
    int frame;
};

/**
 * @brief Calculates the modulo operation, mimicking Python's or Excel's behavior.
 *
 * @param a Dividend.
 * @param b Divisor.
 * @return int The result of the modulo operation.
 */
int mod(int a, int b);

/**
 * @brief Performs floor division, similar to Python's or Excel's behavior.
 *
 * @param dividend The number to be divided.
 * @param divisor The number by which to divide.
 * @return int The floor division result.
 */
int floorDiv(int dividend, int divisor);

/**
 * @brief Computes the position of an arm given a step, arm index, and arm offset.
 *
 * @param step Current step.
 * @param arm Index of the arm.
 * @param arm_offset Offset for the arm.
 * @return int The computed position of the arm.
 */
int compute_postition_of_arm(step_t step, uint8_t arm, uint16_t arm_offset);

/**
 * @brief Computes the column based on the arm position and total steps.
 *
 * @param arm_position The current position of the arm.
 * @param total_steps The total number of steps.
 * @return int The computed column number.
 */
int compute_arm_column(int arm_position, uint8_t total_steps);

/**
 * @brief Calculates the frame offset for an arm position.
 *
 * @param arm_position The current position of the arm.
 * @param total_steps The total number of steps.
 * @return int The computed frame offset.
 */
int compute_frame_offset(int arm_position, step_t total_steps);

/**
 * @brief Computes the frame number based on the arm position and total steps.
 *
 * @param arm_position The current position of the arm.
 * @param total_steps The total number of steps.
 * @return int The computed frame number.
 */
int compute_frame_number(int arm_position, step_t total_steps);

/**
 * @brief Generates a ColumnFrame struct based on the segment, total segments, step, and total steps.
 *
 * @param segment The current segment.
 * @param segments The total number of segments.
 * @param step The current step.
 * @param total_steps The total number of steps.
 * @return ColumnFrame The computed column and frame.
 */
ColumnFrame get_column_frame(uint8_t segment, uint8_t segments, step_t step, step_t total_steps);

// descriptions by chatgpt4. Who knows how correct they are, I haven't read them!

#endif // __UTILS_H__
