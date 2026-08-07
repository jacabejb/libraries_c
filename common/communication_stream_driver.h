/*
 * communication_stream_driver.h
 *
 *  Created on: Aug 6, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#ifndef COMMUNICATION_STREAM_DRIVER_H_
#define COMMUNICATION_STREAM_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>


typedef enum {
	STREAM_OK = 0,
	STREAM_ERROR,
	STREAM_BUSY

} driver_stream_status_t;

typedef struct
{
	driver_stream_status_t (*Write)(void *hw,
                  const uint8_t *buf,
                  uint16_t len);
	driver_stream_status_t (*Busy)(void *hw);
    void (*Flush)(void *hw);

} communication_uart_driver_t;


#endif /* COMMUNICATION_STREAM_DRIVER_H_ */
