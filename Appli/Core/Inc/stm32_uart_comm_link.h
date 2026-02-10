#pragma once

#include "communication/CommLink.h"
#include "cmsis_os2.h"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

class StmUartCommLink : public CommLink<StmUartCommLink> {
public:
    StmUartCommLink();

    void startDma();
    void send(const void* data, size_t len);
    ssize_t receive(void* buffer, size_t len);

    // Called from HAL ISR callbacks — do not call directly
    void onRxEvent(uint16_t size);
    void onTxComplete();

private:
    // DMA receive buffer — placed in non-cacheable RAM via linker section
    static constexpr size_t DMA_BUF_SIZE = 512;
    static uint8_t dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

    // Software ring buffer for received bytes
    static constexpr size_t RING_BUF_SIZE = 1024;
    uint8_t ring_buf_[RING_BUF_SIZE];
    volatile size_t ring_head_;
    volatile size_t ring_tail_;

    // TX completion semaphore
    osSemaphoreId_t tx_sem_;

    // Track last DMA position for partial-transfer handling
    size_t last_rx_pos_;
};
