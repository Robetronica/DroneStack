#include "stm32_debug_log.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include "cmsis_os2.h"
#include "app_freertos.h"
#include "stm32_lcd.h"
#include "stm32n6570_discovery_lcd.h"

extern "C" void ltdc2_set_address_no_reload(uint32_t Address);
extern "C" void ltdc2_reload(void);

extern uint8_t lcd_fg_buffer[800 * 480 * 2];

extern "C" void debug_log(const char* fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0) {
        if (len > static_cast<int>(sizeof(buf) - 1)) {
            len = sizeof(buf) - 1;
        }
        buf[len] = '\0';
        
        if (LcdDebugQueueHandle != NULL) {
            osMessageQueuePut(LcdDebugQueueHandle, buf, 0, 0);
        }
    }
}

static uint32_t current_line = 0;

extern "C" void debug_log_show(const char* msg)
{
    static bool initialized = false;
    const uint32_t font_height = Font20.Height;
    const uint32_t max_lines = 480 / font_height;
    char clean_msg[128];

    if (!initialized) {
        
        /* Set address to our buffer */
        ltdc2_set_address_no_reload((uint32_t)lcd_fg_buffer);
        
        /* Clear with transparency (0x0000 is transparent black in ARGB4444) */
        memset(lcd_fg_buffer, 0, sizeof(lcd_fg_buffer));
        UTIL_LCD_Clear(0x00000000);
        
        UTIL_LCD_SetFont(&Font20);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(0x00000000); // Transparent background for text
        
        UTIL_LCD_DisplayStringAtLine(0, (uint8_t *)"Debug Output Task Started");
        current_line = 1;
        initialized = true;
        ltdc2_reload();

    }

    /* Strip \r and \n */
    strncpy(clean_msg, msg, sizeof(clean_msg));
    for (int i = 0; i < 128 && clean_msg[i] != '\0'; i++)
    {
        if (clean_msg[i] == '\r' || clean_msg[i] == '\n')
        {
            clean_msg[i] = ' ';
        }
    }

    /* Clean and display the new message */
    UTIL_LCD_ClearStringLine(current_line);
    UTIL_LCD_DisplayStringAtLine(current_line, (uint8_t *)clean_msg);

    current_line++;
    if (current_line >= max_lines)
    {
        current_line = 0;
    }
}
