/*
 * button.c
 *
 *  Created on: Aug 9, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#include "button.h"



/***************************************************************************
 * Callback helpers
 ***************************************************************************/

static void Button_CallShortPressed(button_t *obj)
{
    if ((obj != NULL) &&
        (obj->callbacks != NULL) &&
        (obj->callbacks->short_pressed != NULL))
    {
        obj->callbacks->short_pressed(obj);
    }
}

static void Button_CallLongPressed(button_t *obj)
{
    if ((obj != NULL) &&
        (obj->callbacks != NULL) &&
        (obj->callbacks->long_pressed != NULL))
    {
        obj->callbacks->long_pressed(obj);
    }
}

static void Button_CallShortReleased(button_t *obj)
{
    if ((obj != NULL) &&
        (obj->callbacks != NULL) &&
        (obj->callbacks->short_released != NULL))
    {
        obj->callbacks->short_released(obj);
    }
}

static void Button_CallLongReleased(button_t *obj)
{
    if ((obj != NULL) &&
        (obj->callbacks != NULL) &&
        (obj->callbacks->long_released != NULL))
    {
        obj->callbacks->long_released(obj);
    }
}

static void Button_CallRepeated(button_t *obj)
{
    if ((obj != NULL) &&
        (obj->callbacks != NULL) &&
        (obj->callbacks->repeated != NULL))
    {
        obj->callbacks->repeated(obj);
    }
}








static bool Button_IsPressed(
        const button_t *obj)
{
	if(obj ==  NULL){
		return NULL;
	}
	return Driver_GPIO_ReadPin(obj->gpio) == obj->active_state;
}



static void Button_StartPress(
        button_t *obj,
        uint32_t time_ms)
{

	if(obj ==  NULL){
			return;
		}

	obj->status = BUTTON_PRESSED;
	obj->press_start_time = time_ms;
	obj->last_repeat_time = time_ms;

	obj->press_duration = 0;

	obj->short_fired = false;
	obj->long_fired = false;
	obj->repeat_active = false;
}


/**
 * @brief Processes button debounce state.
 *
 * Verifies whether a detected GPIO state change has remained stable
 * for the configured debounce time. If the new state is confirmed,
 * the stable button state is updated and the function returns true.
 *
 * @param obj Pointer to the button object.
 * @param time_ms Current system time in milliseconds.
 *
 * @return true  State change has been accepted after debounce.
 * @return false State change is still being debounced or no change detected.
 */

static bool Button_ProcessDebounce(
        button_t *obj,
        uint32_t time_ms)
{
    driver_gpio_state_t state = obj->gpio->state;

    /* Brak zmiany względem zaakceptowanego stanu */
    if (state == obj->prev_state)
    {
        obj->debounce_active = false;
        return false;
    }

    /* Pierwsze wykrycie zmiany */
    if (!obj->debounce_active)
    {
        obj->debounce_active = true;
        obj->debounce_state = state;
        obj->debounce_start_time = time_ms;
        return false;
    }

    /* Zmiana stanu podczas debounce */
    if (state != obj->debounce_state)
    {
        obj->debounce_state = state;
        obj->debounce_start_time = time_ms;
        return false;
    }

    /* Debounce zakończony */
    if ((time_ms - obj->debounce_start_time) >= obj->debounce_time_ms)
    {
        obj->debounce_active = false;
        obj->prev_state = state;
        return true;
    }

    return false;
}




static void Button_ResetRuntime(button_t *obj)
{
	if(obj ==  NULL){
			return;
		}

	obj->short_fired = false;
	obj->long_fired = false;
	obj->repeat_active = false;

	obj->press_duration = 0;
	obj->press_start_time = 0;
	obj->last_repeat_time = 0;
}


static void Button_HandlePress(
        button_t *obj,
        uint32_t time_ms)
{
	if(obj ==  NULL){
			return;
		}

    obj->press_duration = time_ms - obj->press_start_time;

    /* Short press */
    if ((!obj->short_fired) &&
        (obj->press_duration >= obj->short_press_time_ms))
    {
        obj->short_fired = true;
        obj->status = BUTTON_HELD_SHORT;

        Button_CallShortPressed(obj);
    }

    /* Long press */
    if ((!obj->long_fired) &&
        (obj->press_duration >= obj->long_press_time_ms))
    {
        obj->long_fired = true;
        obj->status = BUTTON_HELD_LONG;

        Button_CallLongPressed(obj);
    }
}


static void Button_HandleRelease(button_t *obj)
{
	if(obj ==  NULL){
			return;
		}

    if (obj->long_fired)
    {
        obj->status = BUTTON_RELEASED_LONG;
        Button_CallLongReleased(obj);
    }
    else if (obj->short_fired)
    {
        obj->status = BUTTON_RELEASED_SHORT;
        Button_CallShortReleased(obj);
    }
    else
    {
        obj->status = BUTTON_IDLE;
    }

    Button_ResetRuntime(obj);
}



static void Button_HandleRepeat(
        button_t *obj,
        uint32_t time_ms)
{
	if(obj ==  NULL){
			return;
		}

    if (!obj->repeat_enable)
    {
        return;
    }

    if (!obj->long_fired)
    {
        return;
    }

    if ((time_ms - obj->press_start_time) <
        (obj->long_press_time_ms + obj->repeat_start_delay_ms))
    {
        return;
    }

    if ((time_ms - obj->last_repeat_time) >=
         obj->repeat_interval_ms)
    {
        obj->last_repeat_time = time_ms;

        obj->repeat_active = true;
        obj->status = BUTTON_REPEAT;

        Button_CallRepeated(obj);
    }
}





button_status_t Button_Init(
        button_t *obj,
        driver_gpio_t *gpio,
        driver_gpio_state_t active_state,
        bool repeat_enable,
        uint16_t debounce_time_ms,
        uint16_t short_press_time_ms,
        uint16_t long_press_time_ms,
        uint16_t repeat_start_delay_ms,
        uint16_t repeat_interval_ms,
        const button_callbacks_t *callbacks,
        void *user_data)
{
    if (obj == NULL)
    {
        return BUTTON_NULL;
    }

    if (gpio == NULL)
    {
        return BUTTON_ERROR;
    }

    obj->gpio = gpio;

    obj->active_state = active_state;

    obj->repeat_enable = repeat_enable;

    obj->debounce_time_ms        = debounce_time_ms;
    obj->short_press_time_ms     = short_press_time_ms;
    obj->long_press_time_ms      = long_press_time_ms;
    obj->repeat_start_delay_ms   = repeat_start_delay_ms;
    obj->repeat_interval_ms      = repeat_interval_ms;

    obj->callbacks = callbacks;
    obj->user_data = user_data;

    obj->prev_state = Driver_GPIO_ReadPin(obj->gpio);

    obj->debounce_state = obj->prev_state;
    obj->debounce_active = false;
    obj->debounce_start_time = 0;

    obj->status = BUTTON_IDLE;

    Button_ResetRuntime(obj);

    return BUTTON_OK;
}





button_status_t Button_Update(
        button_t *obj,
        uint32_t time_ms)
{
    if (obj == NULL)
    {
        return BUTTON_NULL;
    }

    Driver_GPIO_ReadPin(obj->gpio);

    bool pressed = Button_IsPressed(obj);

    /* Zaakceptowano zmianę stanu po debounce */
    if (Button_ProcessDebounce(obj, time_ms))
    {
        if (pressed)
        {
            Button_StartPress(obj, time_ms);
        }
        else
        {
            Button_HandleRelease(obj);
        }
    }

    /* Przycisk jest stabilnie wciśnięty */
    if (obj->prev_state == obj->active_state)
    {
        Button_HandlePress(obj, time_ms);
        Button_HandleRepeat(obj, time_ms);
    }

    return obj->status;
}

void Button_Reset(button_t *obj)
{
    if (obj == NULL)
    {
        return;
    }

    obj->status = BUTTON_IDLE;

    Button_ResetRuntime(obj);

    obj->prev_state = obj->gpio->state;
}


button_status_t Button_GetStatus(const button_t *obj)
{
    if (obj == NULL)
    {
        return BUTTON_NULL;
    }

    return obj->status;
}


void Button_SetDebounceTime(button_t *obj, uint32_t debounce_time)
{
    if (obj == NULL)
    {
        return;
    }

    obj->debounce_time_ms = debounce_time;
}


void Button_SetShortPressTime(button_t *obj, uint32_t short_press_time)
{
    if (obj == NULL)
    {
        return;
    }

    obj->short_press_time_ms = short_press_time;
}


void Button_SetLongPressTime(button_t *obj, uint32_t long_press_time)
{
    if (obj == NULL)
    {
        return;
    }

    obj->long_press_time_ms = long_press_time;
}


void Button_SetRepeatEnable(button_t *obj, button_repeat_t enable)
{
    if (obj == NULL)
    {
        return;
    }

    obj->repeat_enable = enable;
}


void Button_SetRepeatStartDelay(button_t *obj, uint32_t repeat_delay_time)
{
    if (obj == NULL)
    {
        return;
    }

    obj->repeat_start_delay_ms = repeat_delay_time;
}


void Button_SetRepeatInterval(button_t *obj, uint32_t repeat_interval_time)
{
    if (obj == NULL)
    {
        return;
    }

    obj->repeat_interval_ms = repeat_interval_time;
}


void Button_SetCallbacks(button_t *obj, const button_callbacks_t *callbacks)
{
    if (obj == NULL)
    {
        return;
    }

    obj->callbacks = callbacks;
}


void Button_ClearCallbacks(button_t *obj)
{
    if (obj == NULL)
    {
        return;
    }

    obj->callbacks = NULL;
}





WEAK void Button_ShortPressedCallback(button_t *obj)
{
	if (obj == NULL)
	    {
	        return;
	    }

    (void)obj;
}

WEAK void Button_LongPressedCallback(button_t *obj)
{
	if (obj == NULL)
	    {
	        return;
	    }

    (void)obj;
}

WEAK void Button_ShortReleasedCallback(button_t *obj)
{
	if (obj == NULL)
	    {
	        return;
	    }

    (void)obj;
}

WEAK void Button_LongReleasedCallback(button_t *obj)
{
	if (obj == NULL)
	    {
	        return;
	    }

    (void)obj;
}

WEAK void Button_RepeatedCallback(button_t *obj)
{
	if (obj == NULL)
	    {
	        return;
	    }

    (void)obj;
}

