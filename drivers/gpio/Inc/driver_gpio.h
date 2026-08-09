/*
 * driver_gpio.h
 *
 *  Created on: Aug 9, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "driver_gpio_platform.h"



typedef enum
{
    DRIVER_GPIO_RESET = 0,
    DRIVER_GPIO_SET

} driver_gpio_state_t;


typedef enum
{
    DRIVER_GPIO_OK = 0,

    DRIVER_GPIO_NULL,
    DRIVER_GPIO_INVALID_PARAM,
    DRIVER_GPIO_ERROR

} driver_gpio_status_t;

typedef enum
{
    DRIVER_GPIO_MODE_POLLING = 0,
    DRIVER_GPIO_MODE_EXTI

} driver_gpio_mode_t;


typedef struct
{
	driver_gpio_handle_t *handle;
    uint32_t pin;

    driver_gpio_mode_t mode;

    driver_gpio_state_t state;

} driver_gpio_t;




/**
 * @brief Initializes GPIO driver object.
 *
 * Configures the GPIO driver instance and selects the input operating
 * mode (polling or EXTI). The driver object must remain valid for the
 * entire lifetime of the application.
 *
 * @param gpio Pointer to GPIO driver object.
 * @param handle Platform-specific GPIO handle.
 * @param pin GPIO pin identifier.
 * @param mode GPIO operating mode.
 *
 * @return DRIVER_GPIO_OK on success, otherwise an error code.
 */
driver_gpio_status_t Driver_GPIO_Init(
        driver_gpio_t *gpio,
        driver_gpio_handle_t *handle,
        uint32_t pin,
        driver_gpio_mode_t mode);



/**
 * @brief Updates cached GPIO state from an external interrupt.
 *
 * This function shall be called from the platform-specific GPIO interrupt
 * callback when the input is configured in DRIVER_GPIO_MODE_EXTI.
 *
 * For inputs configured in polling mode, this function is not required.
 *
 * @param gpio Pointer to GPIO driver object.
 */
void Driver_GPIO_EXTI_Callback(
        driver_gpio_t *gpio);


driver_gpio_state_t Driver_GPIO_ReadPin(
        driver_gpio_t *gpio);


driver_gpio_status_t Driver_GPIO_WritePin(
        const driver_gpio_t *gpio,
        driver_gpio_state_t state);


driver_gpio_status_t Driver_GPIO_TogglePin(
        const driver_gpio_t *gpio);




#ifdef __cplusplus
}
#endif

#endif /* DRIVER_GPIO_H_ */
