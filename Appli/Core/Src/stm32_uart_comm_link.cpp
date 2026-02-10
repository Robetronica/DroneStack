#include "stm32_uart_comm_link.h"
#include "usart.h"
#include <cstring>

// Static DMA buffer in non-cacheable memory
uint8_t StmUartCommLink::dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

// Singleton pointer for ISR routing
static StmUartCommLink* g_comm_link = nullptr;

StmUartCommLink::StmUartCommLink()
    : ring_head_(0)
    , ring_tail_(0)
    , tx_sem_(nullptr)
    , last_rx_pos_(0)
{
    // Create binary semaphore for TX completion (start released so first send proceeds)
    const osSemaphoreAttr_t sem_attr = { .name = "uartTxSem" };
    tx_sem_ = osSemaphoreNew(1, 1, &sem_attr);
    g_comm_link = this;
}

void StmUartCommLink::startDma()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buf_, DMA_BUF_SIZE);
}

void StmUartCommLink::send(const void* data, size_t len)
{
    if (len == 0) return;

    // Wait for previous TX to complete (blocks the calling task, not the CPU)
    osSemaphoreAcquire(tx_sem_, osWaitForever);
    HAL_UART_Transmit_DMA(&huart2, static_cast<const uint8_t*>(data), static_cast<uint16_t>(len));
}

ssize_t StmUartCommLink::receive(void* buffer, size_t len)
{
    uint8_t* dst = static_cast<uint8_t*>(buffer);
    size_t count = 0;
    size_t tail = ring_tail_;
    size_t head = ring_head_;

    while (count < len && tail != head) {
        dst[count++] = ring_buf_[tail];
        tail = (tail + 1) % RING_BUF_SIZE;
    }
    ring_tail_ = tail;

    return static_cast<ssize_t>(count);
}

void StmUartCommLink::onRxEvent(uint16_t size)
{
    // 'size' is the total number of bytes transferred into dma_rx_buf_ so far
    size_t pos = size;
    size_t head = ring_head_;

    if (pos != last_rx_pos_) {
        size_t start = last_rx_pos_;
        size_t count;

        if (pos > start) {
            count = pos - start;
        } else {
            // Wrapped around
            count = DMA_BUF_SIZE - start + pos;
        }

        for (size_t i = 0; i < count; i++) {
            size_t idx = (start + i) % DMA_BUF_SIZE;
            ring_buf_[head] = dma_rx_buf_[idx];
            head = (head + 1) % RING_BUF_SIZE;
        }
        ring_head_ = head;
        last_rx_pos_ = pos % DMA_BUF_SIZE;
    }

    // Restart DMA if it completed a full buffer
    if (pos >= DMA_BUF_SIZE) {
        last_rx_pos_ = 0;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buf_, DMA_BUF_SIZE);
    }
}

void StmUartCommLink::onTxComplete()
{
    osSemaphoreRelease(tx_sem_);
}

// ---- HAL Callbacks (called from ISR context) ----

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2 && g_comm_link) {
        g_comm_link->onRxEvent(Size);
    }
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2 && g_comm_link) {
        g_comm_link->onTxComplete();
    }
}
