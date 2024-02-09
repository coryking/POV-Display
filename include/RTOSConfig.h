#ifndef RTOS_CONFIG_H_
#define RTOS_CONFIG_H_


#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/message_buffer.h>
#include <freertos/timers.h>
#elif defined(ARDUINO_ARCH_RP2040)
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timer.h>
#include <message_buffer.h>
#else
#error "Platform not supported for FreeRTOS includes"
#endif

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
