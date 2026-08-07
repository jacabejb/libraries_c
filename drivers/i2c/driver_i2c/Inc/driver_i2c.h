/*
 * ads1115_driver_i2c.h
 *
 *  Created on: Aug 3, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#ifndef ADS1115_DRIVER_I2C_H_
#define ADS1115_DRIVER_I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "communication_i2c_driver.h"

/******************************************************************************
 * @brief STM32 HAL blocking I2C communication driver.
 *
 * Ready-to-use implementation of the generic I2C communication interface
 * defined by communication_i2c_driver_t.
 *
 * This driver uses the STM32 HAL blocking I2C functions and can be used
 * by any library requiring the communication_i2c_driver_t interface,
 * for example ADS1115, MCP3424 or other I2C devices.
 *
 * Usage example:
 *
 * @code
 * extern I2C_HandleTypeDef hi2c1;
 *
 * ads1115_t ads;
 *
 * ADS1115_Init(&ads,
 *              &COMMUNICATION_I2C,
 *              &hi2c1,
 *              ADS1115_ADDRESS0);
 * @endcode
 *
 * @note
 * The hardware handle passed to the library must be a pointer to
 * I2C_HandleTypeDef.
 ******************************************************************************/

extern const communication_i2c_driver_t COMMUNICATION_I2C;



#ifdef __cplusplus
}
#endif

#endif /* ADS1115_DRIVER_I2C */
