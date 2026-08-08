/*
 * Console example
 *
 * Basic console usage.
 */

#include "console.h"
#include "driver_uart_dma.h"

extern UART_HandleTypeDef huart2;

console_t console;

int main(void)
{
    HAL_Init();

    /* Initialize peripherals */

    MX_USART2_UART_Init();

    /* Initialize console */

    Console_Init(&console,
                 &COMMUNICATION_UART_DMA,
                 &huart2);

    Console_Printf(&console,
                   "Console initialized.\r\n");

    while (1)
    {
        if (Console_LineReady(&console))
        {
            Console_Printf(&console,
                           "Received: %s\r\n",
                           Console_GetLine(&console));

            Console_LineDone(&console);
        }
    }
}
