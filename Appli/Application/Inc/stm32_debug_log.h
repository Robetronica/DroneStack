#ifndef STM32_DEBUG_LOG_H
#define STM32_DEBUG_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "debug_log.h"

/**
 * @brief Show a debug message on the LCD.
 * 
 * This function processes the message (stripping 
) and renders
 * it on the LCD screen, handling scrolling.
 * 
 * @param msg The message string to display.
 */
void debug_log_show(const char* msg);

#ifdef __cplusplus
}
#endif

#endif // STM32_DEBUG_LOG_H
