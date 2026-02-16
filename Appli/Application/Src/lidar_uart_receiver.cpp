#include "lidar_uart_receiver.h"
#include "usart.h"
#include <cstring>

extern UART_HandleTypeDef huart1;

// Static DMA buffer in non-cacheable memory
uint8_t LidarUartReceiver::dma_rx_buf_[DMA_BUF_SIZE] __attribute__((section(".noncacheable")));

// Singleton pointer for ISR routing
LidarUartReceiver* g_lidar_receiver = nullptr;

LidarUartReceiver::LidarUartReceiver()
    : lidar_(nullptr)
    , scan_count_(0)
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
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dma_rx_buf_, DMA_BUF_SIZE);
    });
}

void LidarUartReceiver::processBytes()
{
    uint8_t chunk[256];
    ssize_t n;

    while ((n = ring_.receive(chunk, sizeof(chunk))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (parser_.feed(chunk[i]) == SerialPacketParser::Result::COMPLETE) {
                handlePacket(parser_.getPacket());
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
