#include "usart.h"
#include "debug_log.h"

// C-callable trampolines defined in C++ modules
extern void px4_comm_link_on_rx_event(uint16_t size);
extern void px4_comm_link_start_dma(void);
extern void px4_comm_link_on_tx_complete(void);
extern void lidar_receiver_on_rx_event(uint16_t size);
extern void lidar_receiver_start_dma(void);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
        px4_comm_link_on_rx_event(Size);
    if (huart->Instance == USART1)
        lidar_receiver_on_rx_event(Size);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
        px4_comm_link_on_tx_complete();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uint32_t err = HAL_UART_GetError(huart);
    
    // Clear error state so we can restart DMA
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
    huart->ErrorCode = HAL_UART_ERROR_NONE;

    const char *tag = (huart->Instance == USART1) ? "LiDAR" :
                      (huart->Instance == USART2) ? "PX4"   : "UART?";

    LOG("[%s] UART ERROR 0x%08X:%s%s%s%s%s\r\n",
        tag, (unsigned)err,
        (err & HAL_UART_ERROR_FE)  ? " FE(framing)"  : "",
        (err & HAL_UART_ERROR_ORE) ? " ORE(overrun)" : "",
        (err & HAL_UART_ERROR_NE)  ? " NE(noise)"    : "",
        (err & HAL_UART_ERROR_PE)  ? " PE(parity)"   : "",
        (err & HAL_UART_ERROR_DMA) ? " DMA"          : "");

    if (huart->Instance == USART1)
        lidar_receiver_start_dma();
    else if (huart->Instance == USART2)
        px4_comm_link_start_dma();
}
