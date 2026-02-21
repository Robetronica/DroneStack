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
    LOG("  USART1 (PE5/PE6) -> PX4 MAVLink\r\n");
    LOG("  USART2 (PD5/PF6) -> LiDAR\r\n");
    LOG("=======================\r\n");
}

static uint32_t uart_diag_tick = 0;
static uint32_t lidar_diag_tick = 0;
static uint32_t last_front_dist_log_tick = 0;

extern "C" void core_app_tick(void)
{
    uint32_t now = osKernelGetTickCount();

    // Log front distance every 10 seconds
    if ((now - last_front_dist_log_tick) >= 10000) {
        last_front_dist_log_tick = now;
        if (lidar_sensor) {
            float dist = lidar_sensor->getDistance(0);
            LOG("[LiDAR] Front Distance: %.2f m\r\n", dist);
        }
    }
    // Periodic UART + connection status every 5 seconds
    if ((now - uart_diag_tick) >= 5000) {
        uart_diag_tick = now;
        size_t lidar_rx = lidar_receiver ? lidar_receiver->getTotalRxBytes() : 0;
        size_t px4_rx   = comm_link ? comm_link->getTotalRxBytes() : 0;
        size_t px4_cbs  = comm_link ? comm_link->getRxCallbackCount() : 0;
        LOG("\r\n--- UART Status ---\r\n");
        LOG("  LiDAR : %u bytes %s\r\n", (unsigned)lidar_rx, lidar_rx > 0 ? "OK" : "NO DATA");
        LOG("  PX4   : %u bytes (cb=%u) %s\r\n",
            (unsigned)px4_rx, (unsigned)px4_cbs, px4_rx > 0 ? "OK" : "NO DATA");
        LOG("-------------------\r\n");
    }

    // Periodic LiDAR parser diagnostics every 5 seconds (until first successful scan)
    if (lidar_receiver && lidar_receiver->getScanCount() == 0 && now > 2000) {
        if ((now - lidar_diag_tick) >= 5000) {
            lidar_diag_tick = now;
            size_t lidar_rx  = lidar_receiver->getTotalRxBytes();
            size_t lidar_fed = lidar_receiver->getBytesFed();
            uint32_t lidar_crc = lidar_receiver->getCrcErrors();
            LOG("[LiDAR] rx=%u fed=%u crc_err=%u scans=0\r\n",
                (unsigned)lidar_rx, (unsigned)lidar_fed, (unsigned)lidar_crc);
            if (lidar_fed > 0 && lidar_crc == 0)
                LOG("  -> No sync found in %u bytes\r\n", (unsigned)lidar_fed);
            else if (lidar_crc > 0)
                LOG("  -> Sync found but CRC failing (%u errors)\r\n", (unsigned)lidar_crc);
        }
    }

    static bool lidar_first_scan_logged = false;
    if (lidar_receiver && lidar_receiver->getScanCount() >= 1 && !lidar_first_scan_logged) {
        lidar_first_scan_logged = true;
        LOG("[LiDAR] First scan received! (rx=%u crc_err=%u)\r\n",
            (unsigned)lidar_receiver->getTotalRxBytes(),
            (unsigned)lidar_receiver->getCrcErrors());
    }

    lidar_receiver->processBytes();
    controller->tick(now);
}
