/*
 * driver_gpio.c
 *
 *  Created on: Aug 9, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#include "driver_gpio.h"





/**
 * @brief Reads the current GPIO hardware level.
 *
 * This function always performs a direct hardware access.
 * It is used internally by both polling and EXTI modes.
 */
static driver_gpio_state_t Driver_GPIO_ReadHardware(
        const driver_gpio_t *gpio)
{
	return (HAL_GPIO_ReadPin(gpio->handle,
	                             (uint16_t)gpio->pin) == GPIO_PIN_SET)
	            ? DRIVER_GPIO_SET
	            : DRIVER_GPIO_RESET;
}




driver_gpio_status_t Driver_GPIO_Init(
        driver_gpio_t *gpio,
        driver_gpio_handle_t *handle,
        uint32_t pin,
        driver_gpio_mode_t mode)
{
	if (gpio == NULL) {
		return DRIVER_GPIO_NULL;
	}

    if (handle == NULL)
    {
        return DRIVER_GPIO_NULL;
    }

    if (mode > DRIVER_GPIO_MODE_EXTI)
    {
        return DRIVER_GPIO_INVALID_PARAM;
    }

    gpio->handle = handle;
    gpio->pin    = pin;
    gpio->mode   = mode;

    /* Odczyt stanu początkowego */

    gpio->state = Driver_GPIO_ReadHardware(gpio);

    return DRIVER_GPIO_OK;
}



driver_gpio_state_t Driver_GPIO_ReadPin(driver_gpio_t *gpio)
{

	if (gpio == NULL)
			return DRIVER_GPIO_RESET;

	if (gpio->mode == DRIVER_GPIO_MODE_POLLING)
	{
		gpio->state =  Driver_GPIO_ReadHardware(gpio);
	}

	return gpio->state;

}



driver_gpio_status_t Driver_GPIO_WritePin(const driver_gpio_t *gpio,
		driver_gpio_state_t state) {

	if (gpio == NULL)
	{
	    return DRIVER_GPIO_NULL;
	}


	HAL_GPIO_WritePin(gpio->handle, (uint16_t) gpio->pin,
			state == DRIVER_GPIO_SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
	return DRIVER_GPIO_OK;
}



driver_gpio_status_t Driver_GPIO_TogglePin(const driver_gpio_t *gpio)
{
	if (gpio == NULL)
	{
	    return DRIVER_GPIO_NULL;
	}


	HAL_GPIO_TogglePin(gpio->handle, (uint16_t) gpio->pin);
	return DRIVER_GPIO_OK;
}




void Driver_GPIO_EXTI_Callback(driver_gpio_t *gpio)
{
	if ((gpio == NULL) || (gpio->mode != DRIVER_GPIO_MODE_EXTI))
	{
	    return;
	}

    gpio->state = Driver_GPIO_ReadHardware(gpio);
}

