/*
 * ads1115_driver_i2c.c
 *
 * Created on: Aug 3, 2026
 * Author: jacabe
 *
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.
 */

#include "driver_i2c.h"

#include "i2c.h"


static driver_status_t ADS1115_IsReady(void *hw,
                                        uint8_t addr)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)hw;

    HAL_StatusTypeDef status =
        HAL_I2C_IsDeviceReady(hi2c, addr << 1, 1, 10);

    switch (status)
    {
        case HAL_OK:
            return DRIVER_OK;

        case HAL_BUSY:
            return DRIVER_BUSY;

        case HAL_TIMEOUT:
            return DRIVER_TIMEOUT;

        case HAL_ERROR:
        default:
            return DRIVER_ERROR;
    }
}

static driver_status_t ADS1115_Write(void *hw,
                                      uint8_t addr,
                                      const uint8_t *data,
                                      uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)hw;

    HAL_StatusTypeDef status =
        HAL_I2C_Master_Transmit(hi2c,
                                addr << 1,
                                (uint8_t *)data,
                                len,
                                10);

    switch (status)
    {
        case HAL_OK:
            return DRIVER_OK;

        case HAL_BUSY:
            return DRIVER_BUSY;

        case HAL_TIMEOUT:
            return DRIVER_TIMEOUT;

        case HAL_ERROR:
        default:
            return DRIVER_ERROR;
    }
}

static driver_status_t ADS1115_Receive(void *hw,
                                        uint8_t addr,
                                        uint8_t *data,
                                        uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)hw;

    HAL_StatusTypeDef status =
        HAL_I2C_Master_Receive(hi2c,
                               addr << 1,
                               data,
                               len,
                               10);

    switch (status)
        {
            case HAL_OK:
                return DRIVER_OK;

            case HAL_BUSY:
                return DRIVER_BUSY;

            case HAL_TIMEOUT:
                return DRIVER_TIMEOUT;

            case HAL_ERROR:
            default:
                return DRIVER_ERROR;
        }
}


const communication_i2c_driver_t COMMUNICATION_I2C =
{
    .IsReady = ADS1115_IsReady,
    .Write   = ADS1115_Write,
    .Receive = ADS1115_Receive
};
