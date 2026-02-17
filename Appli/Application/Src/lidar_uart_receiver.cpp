#include "lidar_uart_receiver.h"
#include "usart.h"
#include "debug_log.h"
#include <cstring>

extern UART_HandleTypeDef huart1;

// Static DMA buffer in non-cacheable memory
uint8_t LidarUartReceiver::dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

// Singleton pointer for ISR routing
LidarUartReceiver* g_lidar_receiver = nullptr;

LidarUartReceiver::LidarUartReceiver()
    : lidar_(nullptr)
    , scan_count_(0)
    , bytes_fed_(0)
{
    processing_buffer_.assign(360, 1000.0f);
    g_lidar_receiver = this;
}

void LidarUartReceiver::setLidar(Lidar2D* lidar)
{
    lidar_ = lidar;
}

void LidarUartReceiver::startDma()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dma_rx_buf_, DMA_BUF_SIZE);
}

void LidarUartReceiver::onRxEvent(uint16_t size)
{
    ring_.onRxEvent(size, dma_rx_buf_, [] {
        // No-op for CIRCULAR mode
    });
}

void LidarUartReceiver::processBytes()
{
    uint8_t chunk[256];
    ssize_t n;
    static bool raw_dumped = false;

    while ((n = ring_.receive(chunk, sizeof(chunk))) > 0) {
        // Dump first 32 raw bytes once to verify wire format
        if (!raw_dumped && n > 0) {
            raw_dumped = true;
            LOG("LIDAR RAW[%d]: ", (int)n);
            for (int j = 0; j < n && j < 32; j++)
                LOG("%02X ", chunk[j]);
            LOG("\r\n");
        }

        bytes_fed_ += static_cast<size_t>(n);
        static uint32_t prev_crc_errors = 0;
        for (ssize_t i = 0; i < n; i++) {
            if (parser_.feed(chunk[i]) == SerialPacketParser::Result::COMPLETE) {
                LOG("[LiDAR] PACKET type=0x%02X len=%u at byte %u\r\n",
                    (unsigned)parser_.getPacket().type,
                    (unsigned)parser_.getPacket().length,
                    (unsigned)bytes_fed_);
                handlePacket(parser_.getPacket());
            }
            // Log first CRC error immediately
            if (parser_.getCrcErrors() > prev_crc_errors) {
                LOG("[LiDAR] CRC FAIL #%u at byte %u\r\n",
                    (unsigned)parser_.getCrcErrors(), (unsigned)bytes_fed_);
                prev_crc_errors = parser_.getCrcErrors();
            }
        }
    }
}

void LidarUartReceiver::handlePacket(const SerialPacketParser::Packet& pkt)
{
    if (pkt.type == SerialPacketType::LIDAR_SCAN) {
        handleLidarScan(pkt.payload, pkt.length);
    }
}

void LidarUartReceiver::handleLidarScan(const uint8_t* payload, uint16_t len)
{
    if (len != sizeof(LidarScanPayload) || !lidar_)
        return;

    const auto* scan = reinterpret_cast<const LidarScanPayload*>(payload);
    for (int i = 0; i < 360; ++i) {
        processing_buffer_[i] = scan->distances[i];
    }

    lidar_->update(
        etl::delegate<void(etl::vector<float, 360>&)>::create<
            LidarUartReceiver, &LidarUartReceiver::applyUpdate>(*this));
    scan_count_++;
}

void LidarUartReceiver::applyUpdate(etl::vector<float, 360>& distances)
{
    distances = processing_buffer_;
}

// C-callable trampoline for HAL callback
extern "C" void lidar_receiver_on_rx_event(uint16_t size)
{
    if (g_lidar_receiver) g_lidar_receiver->onRxEvent(size);
}

extern "C" void lidar_receiver_start_dma(void)
{
    if (g_lidar_receiver) g_lidar_receiver->startDma();
}
