#include "usart.h"

// C-callable trampolines defined in C++ modules
extern void px4_comm_link_on_rx_event(uint16_t size);
extern void px4_comm_link_on_tx_complete(void);
extern void lidar_receiver_on_rx_event(uint16_t size);

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
