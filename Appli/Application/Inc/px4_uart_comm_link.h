#pragma once

#include "uart_dma_link.h"
#include "cmsis_os2.h"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

class PX4UartCommLink {
public:
    PX4UartCommLink();

    void startDma();
    void send(const void* data, size_t len);
    ssize_t receive(void* buffer, size_t len);

    // Called from HAL ISR callbacks — do not call directly
    void onRxEvent(uint16_t size);
    void onTxComplete();

private:
    static constexpr size_t DMA_BUF_SIZE = 512;
    static uint8_t dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

    UartDmaRingBuffer<DMA_BUF_SIZE, 1024> ring_;
    osSemaphoreId_t tx_sem_;
};
