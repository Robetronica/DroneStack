#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

/// Reusable DMA-to-ring-buffer engine for UART reception.
/// Used via composition — each UART peripheral owns one instance.
template <size_t DmaBufSize, size_t RingBufSize>
class UartDmaRingBuffer {
public:
    /// Called from ISR context when DMA has transferred bytes.
    /// @param size     total bytes transferred into dma_buf so far
    /// @param dma_buf  pointer to the DMA receive buffer
    /// @param restart  callable invoked when a full-buffer wrap requires DMA restart
    template <typename RestartFn>
    void onRxEvent(uint16_t size, const uint8_t* dma_buf, RestartFn restart)
    {
        size_t pos = size;
        size_t head = ring_head_;

        if (pos != last_rx_pos_) {
            size_t start = last_rx_pos_;
            size_t count;

            if (pos > start) {
                count = pos - start;
            } else {
                count = DmaBufSize - start + pos;
            }

            for (size_t i = 0; i < count; i++) {
                size_t idx = (start + i) % DmaBufSize;
                ring_buf_[head] = dma_buf[idx];
                head = (head + 1) % RingBufSize;
            }
            ring_head_ = head;
            last_rx_pos_ = pos % DmaBufSize;
        }

        if (pos >= DmaBufSize) {
            last_rx_pos_ = 0;
            restart();
        }
    }

    /// Drain bytes from ring buffer into caller's buffer (task context).
    ssize_t receive(void* buf, size_t len)
    {
        uint8_t* dst = static_cast<uint8_t*>(buf);
        size_t count = 0;
        size_t tail = ring_tail_;
        size_t head = ring_head_;

        while (count < len && tail != head) {
            dst[count++] = ring_buf_[tail];
            tail = (tail + 1) % RingBufSize;
        }
        ring_tail_ = tail;

        return static_cast<ssize_t>(count);
    }

private:
    uint8_t ring_buf_[RingBufSize];
    volatile size_t ring_head_ = 0;
    volatile size_t ring_tail_ = 0;
    size_t last_rx_pos_ = 0;
};
