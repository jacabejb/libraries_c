/*
 * ads1115_driver_i2c_it.h
 *
 *  Created on: Aug 4, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 * @brief HAL callback handlers.
 *
 * These functions must be called from the corresponding STM32 HAL I2C
 * callbacks located in stm32xxxx_hal_i2c.c or user main.c.
 *
 * Example:
 *
 * void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
 * {
 *     ADS1115_Driver_I2C_IT_TxCpltCallback(&ads_i2c, hi2c);
 * }
 *
 * void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
 * {
 *     ADS1115_Driver_I2C_IT_RxCpltCallback(&ads_i2c, hi2c);
 * }
 *
 * void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
 * {
 *     ADS1115_Driver_I2C_IT_ErrorCallback(&ads_i2c, hi2c);
 * }
 *
 */
#ifndef DRIVER_I2C_IT_H
#define DRIVER_I2C_IT_H

#include "communication_i2c_driver.h"
#include "i2c.h"

typedef enum
{
    I2C_STATE_IDLE = 0,
    I2C_STATE_BUSY,
    I2C_STATE_DONE,
    I2C_STATE_ERROR

} i2c_state_t;

typedef struct
{
    I2C_HandleTypeDef *hi2c;

    volatile i2c_state_t state;

    HAL_StatusTypeDef hal_status;

} driver_i2c_it_t;

/* driver object */

/******************************************************************************
 * @brief STM32 HAL interrupt-based I2C communication driver.
 *
 * Ready-to-use implementation of the generic I2C communication interface
 * defined by communication_i2c_driver_t.
 *
 * This driver uses the STM32 HAL interrupt I2C functions and can be used
 * by any library requiring the communication_i2c_driver_t interface,
 * for example ADS1115, MCP3424 or other I2C devices.
 *
 * Before using this driver, a driver_i2c_it_t object must be initialized
 * by calling Driver_I2C_Init().
 *
 * The STM32 HAL I2C callbacks must forward their events to the driver
 * callback functions declared below.
 *
 * Usage example:
 *
 * @code
 * driver_i2c_it_t drv;
 *
 * Driver_I2C_Init(&drv, &hi2c1);
 *
 * ADS1115_Init(&ads,
 *              &COMMUNICATION_I2C_IT,
 *              &drv,
 *              ADS1115_ADDRESS0);
 * @endcode
 *
 * @note
 * The hardware handle passed to the library must be a pointer to an
 * initialized driver_i2c_it_t object.
 ******************************************************************************/
extern const communication_i2c_driver_t COMMUNICATION_I2C_IT;



/* initialization */

void Driver_I2C_IT_Init(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c);

/* HAL callbacks */

void Driver_I2C_IT_TxCpltCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c);

void Driver_I2C_IT_RxCpltCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c);

void Driver_I2C_IT_ErrorCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c);

#endif
