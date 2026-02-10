#include "stm32_core_app.h"
#include "stm32_uart_comm_link.h"
#include "OffboardControl.h"
#include "cmsis_os2.h"
#include <new>
#include <cstdint>

// Static storage — avoids FreeRTOS heap allocation for these large objects
using CommLinkType = StmUartCommLink;
using ControllerType = OffboardControl<CommLinkType>;

static uint8_t comm_link_buf[sizeof(CommLinkType)] __attribute__((aligned(8), section(".psram_bss")));
static uint8_t controller_buf[sizeof(ControllerType)] __attribute__((aligned(8), section(".psram_bss")));  

static CommLinkType* comm_link = nullptr;
static ControllerType* controller = nullptr;

extern "C" void core_app_init(void)
{
    comm_link = new (comm_link_buf) CommLinkType();
    comm_link->startDma();

    controller = new (controller_buf) ControllerType(comm_link);
    controller->start(osKernelGetTickCount());
}

extern "C" void core_app_tick(void)
{
    controller->tick(osKernelGetTickCount());
}
