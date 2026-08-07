#ifndef DRIVER_USB_H_
#define DRIVER_USB_H_

#include "communication_stream_driver.h"

#include <stdbool.h>


/******************************************************************************
 * @brief STM32 HAL USB CDC communication driver.
 *
 * Ready-to-use implementation of the generic UART communication interface
 * defined by communication_uart_driver_t.
 *
 * This driver uses the STM32 HAL USB CDC interface and can be used
 * by any library requiring the communication_uart_driver_t interface,
 * for example the Console library.
 *
 * Usage example:
 *
 * @code
 * console_t console;
 *
 * Console_Init(&console,
 *              &COMMUNICATION_DRIVER_USB,
 *              NULL);
 * @endcode
 *
 * @note
 * This driver uses the STM32 USB Device CDC middleware.
 * The hardware handle parameter is not used and should be set to NULL.
 ******************************************************************************/
extern const communication_uart_driver_t COMMUNICATION_USB;

#endif /* DRIVER_USB_H_ */
