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

    size_t getTotalRxBytes() const { return ring_.getTotalRxBytes(); }
    size_t getRxCallbackCount() const { return rx_callback_count_; }

    // Called from HAL ISR callbacks — do not call directly
    void onRxEvent(uint16_t size);
    void onTxComplete();

private:
    static constexpr size_t RX_BUF_SIZE = 512;
    static constexpr size_t TX_BUF_SIZE  = 280; // MAVLINK_MAX_PACKET_LEN

    // Both buffers in non-cacheable memory: DMA reads/writes bypass L1 cache
    // Declares AND defines the variable right here.
    inline static uint8_t dma_rx_buf_[RX_BUF_SIZE] __attribute__((section(".noncacheable"), aligned(32)));
    inline static uint8_t dma_tx_buf_[TX_BUF_SIZE]  __attribute__((section(".noncacheable"), aligned(32)));
 
    UartDmaRingBuffer<RX_BUF_SIZE, 1024> ring_;
    osSemaphoreId_t tx_sem_;
    volatile size_t rx_callback_count_ = 0;
};
