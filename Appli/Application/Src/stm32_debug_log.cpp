#include "debug_log.h"
#include "usart.h"
#include <cstdio>
#include <cstring>

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
        HAL_UART_Transmit(&huart1, reinterpret_cast<const uint8_t*>(buf),
                          static_cast<uint16_t>(len), 50);
    }
}
