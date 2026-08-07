/*
 * communications_i2c_driver.h
 *
 *  Created on: Aug 6, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#ifndef COMMUNICATIONS_I2C_DRIVER_H_
#define COMMUNICATIONS_I2C_DRIVER_H_

#include <stdint.h>

typedef enum
{
    DRIVER_OK = 0,
    DRIVER_BUSY,
	DRIVER_TIMEOUT,
    DRIVER_ERROR
} driver_status_t;

typedef struct
{
    driver_status_t (*IsReady)(void *hw,
                               uint8_t addr);

    driver_status_t (*Write)(void *hw,
                             uint8_t addr,
                             const uint8_t *data,
                             uint16_t len);

    driver_status_t (*Receive)(void *hw,
                               uint8_t addr,
                               uint8_t *data,
                               uint16_t len);

} communication_i2c_driver_t;


#endif /* COMMUNICATIONS_I2C_DRIVER_H_ */
