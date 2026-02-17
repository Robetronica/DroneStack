#pragma once

#include "uart_dma_link.h"
#include "communication/SerialPacketParser.h"
#include "sensors/Lidar2D.h"
#include <cstdint>
#include <cstddef>

class LidarUartReceiver {
public:
    LidarUartReceiver();

    void setLidar(Lidar2D* lidar);
    void startDma();

    // Call from task context at >= 10 Hz to drain ring buffer and parse packets
    void processBytes();

    // Called from HAL ISR — do not call directly
    void onRxEvent(uint16_t size);

    uint32_t getScanCount() const { return scan_count_; }
    uint32_t getCrcErrors() const { return parser_.getCrcErrors(); }
    size_t getTotalRxBytes() const { return ring_.getTotalRxBytes(); }
    size_t getBytesFed() const { return bytes_fed_; }

private:
    static constexpr size_t DMA_BUF_SIZE = 2048;
    static uint8_t dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable"), aligned(32)));

    UartDmaRingBuffer<DMA_BUF_SIZE, 3072> ring_;
    SerialPacketParser parser_;
    Lidar2D* lidar_;
    uint32_t scan_count_;
    size_t bytes_fed_;

    void handlePacket(const SerialPacketParser::Packet& pkt);
    void handleLidarScan(const uint8_t* payload, uint16_t len);

    etl::vector<float, 360> processing_buffer_;
    void applyUpdate(etl::vector<float, 360>& distances);
};

// Singleton pointer — accessed by HAL callbacks
extern LidarUartReceiver* g_lidar_receiver;
