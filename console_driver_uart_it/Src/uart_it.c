#include "console.h"
#include "usart.h"

static bool UART_IT_Write(void *hw,
                          const uint8_t *buf,
                          uint16_t len)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    return HAL_UART_Transmit_IT(huart,
                                (uint8_t *)buf,
                                len) == HAL_OK;
}

static bool UART_IT_Busy(void *hw)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    return (huart->gState != HAL_UART_STATE_READY);
}

static void UART_IT_Flush(void *hw)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)hw;

    while (huart->gState != HAL_UART_STATE_READY)
    {
    }
}

const console_driver_t ConsoleDriverUART_IT =
{
    .Write = UART_IT_Write,
    .Busy  = UART_IT_Busy,
    .Flush = UART_IT_Flush
};