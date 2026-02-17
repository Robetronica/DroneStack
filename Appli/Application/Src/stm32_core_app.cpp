#include "stm32_core_app.h"
#include "px4_uart_comm_link.h"
#include "lidar_uart_receiver.h"
#include "OffboardControl.h"
#include "sensors/Lidar2D.h"
#include "debug_log.h"
#include "cmsis_os2.h"
#include <new>
#include <cstdint>

// Static storage — avoids FreeRTOS heap allocation for these large objects
using CommLinkType = PX4UartCommLink;
using ControllerType = OffboardControl<CommLinkType>;

static uint8_t comm_link_buf[sizeof(CommLinkType)] __attribute__((aligned(8), section(".psram_bss")));
static uint8_t controller_buf[sizeof(ControllerType)] __attribute__((aligned(8), section(".psram_bss")));
static uint8_t lidar_receiver_buf[sizeof(LidarUartReceiver)] __attribute__((aligned(8), section(".psram_bss")));
static uint8_t lidar_sensor_buf[sizeof(Lidar2D)] __attribute__((aligned(8), section(".psram_bss")));

static CommLinkType* comm_link = nullptr;
static ControllerType* controller = nullptr;
static LidarUartReceiver* lidar_receiver = nullptr;
static Lidar2D* lidar_sensor = nullptr;

extern "C" void core_app_init(void)
{
    // MAVLink comm link (USART2)
    comm_link = new (comm_link_buf) CommLinkType();
    comm_link->startDma();

    // LiDAR sensor and UART receiver (USART1)
    lidar_sensor = new (lidar_sensor_buf) Lidar2D();
    lidar_receiver = new (lidar_receiver_buf) LidarUartReceiver();
    lidar_receiver->setLidar(lidar_sensor);
    lidar_receiver->startDma();

    controller = new (controller_buf) ControllerType(comm_link);
    controller->setSensor(lidar_sensor);
    controller->start(osKernelGetTickCount());

    LOG("\r\n=== UART Assignment ===\r\n");
    LOG("  USART1 (PE5/PE6) -> LiDAR\r\n");
    LOG("  USART2 (PD5/PF6) -> PX4 MAVLink\r\n");
    LOG("=======================\r\n");
}

static bool uart_check_done = false;
static uint32_t uart_check_tick = 0;

extern "C" void core_app_tick(void)
{
    uint32_t now = osKernelGetTickCount();

    if (!uart_check_done) {
        if (uart_check_tick == 0) {
            uart_check_tick = now;
        } else if ((now - uart_check_tick) >= 2000) {
            uart_check_done = true;
            size_t lidar_rx = lidar_receiver->getTotalRxBytes();
            size_t px4_rx = comm_link->getTotalRxBytes();

            LOG("\r\n--- UART Status (2s) ---\r\n");
            LOG("  USART1 LiDAR : %u bytes %s\r\n",
                (unsigned)lidar_rx, lidar_rx > 0 ? "OK" : "NO DATA");
            LOG("  USART2 PX4   : %u bytes %s\r\n",
                (unsigned)px4_rx, px4_rx > 0 ? "OK" : "NO DATA");

            if (lidar_rx == 0 && px4_rx == 0)
                LOG("  -> No data on either UART. Check connections.\r\n");
            else if (lidar_rx == 0 && px4_rx > 0)
                LOG("  -> LiDAR silent. Cables swapped?\r\n");
            else if (lidar_rx > 0 && px4_rx == 0)
                LOG("  -> PX4 silent. Cables swapped?\r\n");

            LOG("------------------------\r\n");
        }
    }

    lidar_receiver->processBytes();
    controller->tick(now);
}
