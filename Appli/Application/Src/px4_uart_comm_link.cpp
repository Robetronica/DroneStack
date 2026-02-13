#include "px4_uart_comm_link.h"
#include "usart.h"
#include <cstring>

extern UART_HandleTypeDef huart2;

// Static DMA buffer in non-cacheable memory
uint8_t PX4UartCommLink::dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

// Singleton pointer for ISR routing
static PX4UartCommLink* g_comm_link = nullptr;

PX4UartCommLink::PX4UartCommLink()
    : tx_sem_(nullptr)
{
    const osSemaphoreAttr_t sem_attr = { .name = "uartTxSem" };
    tx_sem_ = osSemaphoreNew(1, 1, &sem_attr);
    g_comm_link = this;
}

void PX4UartCommLink::startDma()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buf_, DMA_BUF_SIZE);
}

void PX4UartCommLink::send(const void* data, size_t len)
{
    if (len == 0) return;

    osSemaphoreAcquire(tx_sem_, osWaitForever);
    HAL_UART_Transmit_DMA(&huart2, static_cast<const uint8_t*>(data), static_cast<uint16_t>(len));
}

ssize_t PX4UartCommLink::receive(void* buffer, size_t len)
{
    return ring_.receive(buffer, len);
}

void PX4UartCommLink::onRxEvent(uint16_t size)
{
    ring_.onRxEvent(size, dma_rx_buf_, [] {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buf_, DMA_BUF_SIZE);
    });
}

void PX4UartCommLink::onTxComplete()
{
    osSemaphoreRelease(tx_sem_);
}

// C-callable trampolines for HAL callbacks
extern "C" void px4_comm_link_on_rx_event(uint16_t size)
{
    if (g_comm_link) g_comm_link->onRxEvent(size);
}

extern "C" void px4_comm_link_on_tx_complete(void)
{
    if (g_comm_link) g_comm_link->onTxComplete();
}
