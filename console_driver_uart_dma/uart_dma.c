#include "console.h"
#include "usart.h"

static bool UART_DMA_Write(void *hw,
                           const uint8_t *buf,
                           uint16_t len)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    return HAL_UART_Transmit_DMA(huart,
                                 (uint8_t *)buf,
                                 len) == HAL_OK;
}

static bool UART_DMA_Busy(void *hw)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    return (huart->gState != HAL_UART_STATE_READY);
}

static void UART_DMA_Flush(void *hw)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    while (huart->gState != HAL_UART_STATE_READY)
    {
    }
}

const console_driver_t ConsoleDriverUART_DMA =
{
    .Write = UART_DMA_Write,
    .Busy  = UART_DMA_Busy,
    .Flush = UART_DMA_Flush
};