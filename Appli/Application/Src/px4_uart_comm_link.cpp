#include "px4_uart_comm_link.h"
#include "usart.h"
#include "debug_log.h"
#include <cstring>

extern UART_HandleTypeDef huart2;

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
    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buf_, DMA_BUF_SIZE);
    if (status != HAL_OK) {
        LOG("[PX4] startDma FAILED: HAL status=%d gState=0x%02X RxState=0x%02X err=0x%08X\r\n",
            (int)status,
            (unsigned)huart2.gState,
            (unsigned)huart2.RxState,
            (unsigned)huart2.ErrorCode);
    } else {
        LOG("[PX4] startDma OK: gState=0x%02X RxState=0x%02X\r\n",
    

        (unsigned)huart2.gState,
            (unsigned)huart2.RxState);
    }
}

void PX4UartCommLink::send(const void* data, size_t len)
{
    if (len == 0 || len > TX_BUF_SIZE) return;

    osSemaphoreAcquire(tx_sem_, osWaitForever);
    // Copy into non-cacheable buffer so DMA reads coherent data on Cortex-M55
    memcpy(dma_tx_buf_, data, len);
    HAL_UART_Transmit_DMA(&huart2, dma_tx_buf_, static_cast<uint16_t>(len));
}

ssize_t PX4UartCommLink::receive(void* buffer, size_t len)
{
    return ring_.receive(buffer, len);
}

void PX4UartCommLink::onRxEvent(uint16_t size)
{
    rx_callback_count_++;
    ring_.onRxEvent(size, dma_rx_buf_, [] {
        // No-op for CIRCULAR mode
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

extern "C" void px4_comm_link_start_dma(void)
{
    if (g_comm_link) g_comm_link->startDma();
}

extern "C" void px4_comm_link_on_tx_complete(void)
{
    if (g_comm_link) g_comm_link->onTxComplete();
}
