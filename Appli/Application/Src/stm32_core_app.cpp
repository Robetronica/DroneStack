#include "stm32_core_app.h"
#include "px4_uart_comm_link.h"
#include "lidar_uart_receiver.h"
#include "OffboardControl.h"
#include "sensors/Lidar2D.h"
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

    // LiDAR sensor and UART receiver (UART4)
    lidar_sensor = new (lidar_sensor_buf) Lidar2D();
    lidar_receiver = new (lidar_receiver_buf) LidarUartReceiver();
    lidar_receiver->setLidar(lidar_sensor);
    lidar_receiver->startDma();

    controller = new (controller_buf) ControllerType(comm_link);
    controller->setSensor(lidar_sensor);
    controller->start(osKernelGetTickCount());
}

extern "C" void core_app_tick(void)
{
    lidar_receiver->processBytes();
    controller->tick(osKernelGetTickCount());
}
