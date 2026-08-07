#ifndef DRIVER_UART_IT_H_
#define DRIVER_UART_IT_H_

#include "communication_stream_driver.h"

/******************************************************************************
 * @brief STM32 HAL UART interrupt communication driver.
 *
 * Ready-to-use implementation of the generic UART communication interface
 * defined by communication_uart_driver_t.
 *
 * This driver uses the STM32 HAL DMA UART functions and can be used
 * by any library requiring the communication_uart_driver_t interface,
 * for example the Console library.
 *
 * Usage example:
 *
 * @code
 * console_t console;
 *
 * Console_Init(&console,
 *              &COMMUNICATION_UART_DMA,
 *              &huart2);
 * @endcode
 *
 * @note
 * The hardware handle passed to the library must be a pointer to
 * UART_HandleTypeDef.
 ******************************************************************************/
extern const communication_uart_driver_t COMMUNICATION_DRIVER_UART_IT;

#endif /* DRIVER_UART_IT_H_ */