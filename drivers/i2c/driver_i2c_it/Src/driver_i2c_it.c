/*
 * ads1115_driver_i2c_it.c
 *
 *  Created on: Aug 4, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#include <string.h>
#include "driver_i2c_it.h"



void Driver_I2C_IT_Init(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c)
{
	memset(drv,0,sizeof(*drv));

    drv->hi2c = hi2c;

    drv->state = I2C_STATE_IDLE;

    drv->hal_status = HAL_OK;
}

/******************************************************************************
 * These callbacks do not need to be called from ADS1115_Update().
 *
 * They are invoked automatically by STM32 HAL interrupt handlers.
 ******************************************************************************/
void Driver_I2C_IT_TxCpltCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c)
{
    if (drv->hi2c != hi2c)
        return;

    drv->state = I2C_STATE_DONE;
}

void Driver_I2C_IT_RxCpltCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c)
{
    if (drv->hi2c != hi2c)
        return;

    drv->state = I2C_STATE_DONE;
}

void Driver_I2C_IT_ErrorCallback(
        driver_i2c_it_t *drv,
        I2C_HandleTypeDef *hi2c)
{
    if (drv->hi2c != hi2c)
        return;

    drv->hal_status = hi2c->ErrorCode;
    drv->state = I2C_STATE_ERROR;
}



/******************************************************************************
 * @brief Checks whether the ADS1115 device responds on the I2C bus.
 *
 * @param hw
 * Pointer to driver hardware object.
 *
 * @param address
 * 7-bit I2C device address.
 *
 * @return
 * ADS1115_DRIVER_OK     Device is present.
 * ADS1115_DRIVER_BUSY   I2C peripheral is busy.
 * ADS1115_DRIVER_ERROR  Device did not respond.
 ******************************************************************************/
static driver_status_t driver_IsReady(void *hw,
                                               uint8_t address)
{
    driver_i2c_it_t *drv = (driver_i2c_it_t *)hw;

    HAL_StatusTypeDef status;

    status = HAL_I2C_IsDeviceReady(drv->hi2c,
                                   address << 1,
                                   1,
                                   10);

    switch (status)
    {
        case HAL_OK:
            return DRIVER_OK;

        case HAL_BUSY:
            return DRIVER_BUSY;

        default:
            return DRIVER_ERROR;
    }
}

static driver_status_t driver_Write(void *hw,
                          uint8_t address,
                          const uint8_t *data,
                          uint16_t length)
{
    driver_i2c_it_t *drv = hw;

    switch (drv->state)
    {
        /*------------------------------------------------------*/
        case I2C_STATE_IDLE:

            drv->hal_status =
                HAL_I2C_Master_Transmit_IT(drv->hi2c,
                                           address << 1,
                                           (uint8_t *)data,
                                           length);

            if (drv->hal_status == HAL_BUSY)
                return DRIVER_BUSY;

            if (drv->hal_status != HAL_OK)
            {
                drv->state = I2C_STATE_ERROR;
                return DRIVER_ERROR;
            }

            drv->state = I2C_STATE_BUSY;
            return DRIVER_BUSY;      /* Busy */

        /*------------------------------------------------------*/
        case I2C_STATE_BUSY:

            return DRIVER_BUSY;      /* Still busy */

        /*------------------------------------------------------*/
        case I2C_STATE_DONE:

            drv->state = I2C_STATE_IDLE;
            return DRIVER_OK;

        /*------------------------------------------------------*/
        case I2C_STATE_ERROR:

            drv->state = I2C_STATE_IDLE;
            return DRIVER_ERROR;

        default:

            drv->state = I2C_STATE_IDLE;
            return DRIVER_ERROR;
    }
}


static driver_status_t driver_Receive(void *hw, uint8_t address,
		uint8_t *data, uint16_t length) {

	driver_i2c_it_t *drv = hw;

	switch (drv->state) {
	/*------------------------------------------------------*/
	case I2C_STATE_IDLE:

		drv->hal_status = HAL_I2C_Master_Receive_IT(drv->hi2c, address << 1,
				(uint8_t*) data, length);

		if (drv->hal_status == HAL_BUSY)
		    return DRIVER_BUSY;

		if (drv->hal_status != HAL_OK) {
			drv->state = I2C_STATE_ERROR;
			return DRIVER_ERROR;
		}

		drv->state = I2C_STATE_BUSY;
		return DRIVER_BUSY; /* Busy */

		/*------------------------------------------------------*/
	case I2C_STATE_BUSY:

		return DRIVER_BUSY; /* Still busy */

		/*------------------------------------------------------*/
	case I2C_STATE_DONE:

		drv->state = I2C_STATE_IDLE;
		return DRIVER_OK;

		/*------------------------------------------------------*/
	case I2C_STATE_ERROR:

		drv->state = I2C_STATE_IDLE;
		return DRIVER_ERROR;

	default:

		drv->state = I2C_STATE_IDLE;
		return DRIVER_ERROR;
	}
}


const communication_i2c_driver_t COMMUNICATION_I2C_IT =
{
    .IsReady = driver_IsReady,
    .Write   = driver_Write,
    .Receive = driver_Receive
};
