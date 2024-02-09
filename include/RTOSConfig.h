#ifndef RTOS_CONFIG_H_
#define RTOS_CONFIG_H_

namespace RTOS {

// Priorities (adjust as needed for your system)
constexpr UBaseType_t LOW_PRIORITY = 5;
constexpr UBaseType_t NORMAL_PRIORITY = 10;
constexpr UBaseType_t HIGH_PRIORITY = 20;

// Stack sizes (adjust based on task complexity)
constexpr configSTACK_DEPTH_TYPE SMALL_STACK_SIZE = 1024*2;
constexpr configSTACK_DEPTH_TYPE MEDIUM_STACK_SIZE = 1024*3;
constexpr configSTACK_DEPTH_TYPE LARGE_STACK_SIZE = 1024*6;
constexpr configSTACK_DEPTH_TYPE XLARGE_STACK_SIZE = 1024 * 12;

} // namespace RTOS

#endif // RTOS_CONFIG_H_
