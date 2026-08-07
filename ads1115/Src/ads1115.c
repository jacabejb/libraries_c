/*
 * ads1115.c
 *
 *  Created on: Aug 3, 2026
 *      Author: jacabe
 *
 * @brief ADS1115 non-blocking driver.
 *
 * Part of STM32 Universal Libraries
 *
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.
 */



#include "ads1115.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


//static ads1115_status_t ADS1115_StartConversion(ads1115_t *ads);
//static uint16_t ADS1115_BuildConfig(const ads1115_t *ads);


static const uint16_t MUX_BY_CHANNEL[] = {
    ADS1X15_REG_CONFIG_MUX_SINGLE_0, ///< Single-ended AIN0
    ADS1X15_REG_CONFIG_MUX_SINGLE_1, ///< Single-ended AIN1
    ADS1X15_REG_CONFIG_MUX_SINGLE_2, ///< Single-ended AIN2
    ADS1X15_REG_CONFIG_MUX_SINGLE_3  ///< Single-ended AIN3
};                                   ///< MUX config by channel

static const uint16_t GAIN_BY_ENUM[] =
{
    ADS1X15_REG_CONFIG_PGA_6_144V,
    ADS1X15_REG_CONFIG_PGA_4_096V,
    ADS1X15_REG_CONFIG_PGA_2_048V,
    ADS1X15_REG_CONFIG_PGA_1_024V,
    ADS1X15_REG_CONFIG_PGA_0_512V,
    ADS1X15_REG_CONFIG_PGA_0_256V
};

static const uint16_t RATE_BY_ENUM[] =
{
    0x0000, // 8 SPS
    0x0020, // 16 SPS
    0x0040, // 32 SPS
    0x0060, // 64 SPS
    0x0080, // 128 SPS
    0x00A0, // 250 SPS
    0x00C0, // 475 SPS
    0x00E0  // 860 SPS
};

static const uint8_t CONVERSION_TIME_BY_RATE[] =
{
    125,    // 8 SPS
     63,    // 16 SPS
     32,    // 32 SPS
     16,    // 64 SPS
      8,    // 128 SPS
      4,    // 250 SPS
      3,    // 475 SPS
      2     // 860 SPS
};

static const int16_t FULL_SCALE_MV_BY_GAIN[] =
{
    6144,
    4096,
    2048,
    1024,
    512,
    256
};


/******************************************************************************
 * @brief Converts driver status to ADS1115 status.
 *
 * @param drv_status
 * Driver communication status.
 *
 * @return
 * ADS1115 library status.
 ******************************************************************************/
static ads1115_status_t ADS1115_DriverStatusToStatus(
        ads1115_driver_status_t drv_status)
{
    switch (drv_status)
    {
        case ADS1115_DRIVER_OK:
            return ADS1115_OK;

        case ADS1115_DRIVER_BUSY:
            return ADS1115_BUSY;

        default:
            return ADS1115_ERROR;
    }
}

/*mV = raw × FullScale_mV / 32768*/

static int16_t ADS1115_RawToMilliVolts(const ads1115_t *obj,
                                       int16_t raw)
{
	return (raw * FULL_SCALE_MV_BY_GAIN[obj->gain]) / ADS1115_ADC_FULL_SCALE;

}

ads1115_status_t ADS1115_Init(ads1115_t *obj, const communication_i2c_driver_t *driver,
		void *hw, uint8_t address) {


	if ((obj == NULL) || (driver == NULL)) {
		return ADS1115_ERROR;
	}

	if ((driver->Write == NULL) || (driver->Receive == NULL) || (driver->IsReady == NULL)) {
		obj->status = ADS1115_ERROR;
		return obj->status;
	}

	obj->driver = driver;
	obj->hw = hw;
	obj->address = address;

	obj->current_channel = 0;
	obj->conversion_start_time = 0;
	obj->min_conversion_time = 0;
	obj->flags = 0;
	obj->cycles = 0;

	obj->state = ADS1115_STATE_IDLE;
	obj->status = ADS1115_IDLE;

	memset(obj->raw, 0, sizeof(obj->raw));
	memset(obj->voltage, 0, sizeof(obj->voltage));

	return obj->status;
}

ads1115_status_t ADS1115_Start(ads1115_t *obj,
                               ads1115_mode_t mode,
                               ads1115_scan_t scan,
                               uint8_t channel,
                               uint8_t cycles,
                               uint32_t time_now)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    if (scan == ADS1115_SCAN_SINGLE_CHANNEL)
    {
        if (channel >= ADS1115_CHANNELS)
        {
        	obj->diagnostic.invalid_channel++;
        	obj->status = ADS1115_INVALID_CHANNEL;
            return obj->status;
        }

        obj->current_channel = channel;
    }
    else
    {
    	obj->current_channel = 0;
    }

    if (mode == ADS1115_MODE_ONESHOT)
    {
        if (cycles == 0)
        {
        	obj->diagnostic.invalid_parameter++;
        	obj->status = ADS1115_INVALID_PARAMETER;
            return obj->status;
        }

        obj->cycles = cycles;
    }
    else
    {
        /* Ignored in continuous mode */
    	obj->cycles = 0;
    }

    obj->mode = mode;
    obj->scan = scan;


    obj->cycles = cycles;

    obj->flags &= ~ADS1115_FLAG_STOP_REQUEST;

    obj->conversion_start_time = time_now;

    obj->state = ADS1115_STATE_START_CONVERSION;

    return obj->status;
}

ads1115_status_t ADS1115_Stop(ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    /* Already idle */
    if (obj->state == ADS1115_STATE_IDLE)
        return obj->status;

    /* Request graceful stop */
    obj->flags |= ADS1115_FLAG_STOP_REQUEST;

    return obj->status;
}

/******************************************************************************
 * @brief Builds ADS1115 CONFIG register from object settings.
 *
 * Creates a complete CONFIG register based on the current object state.
 * The register is generated on demand and is not stored in the object.
 *
 * @param ads Pointer to ADS1115 object.
 *
 * @return CONFIG register value.
 ******************************************************************************/
static uint16_t ADS1115_BuildConfig(const ads1115_t *obj)
{
    uint16_t config;

    /* Common configuration */

    config =
        GAIN_BY_ENUM[obj->gain]             |
        RATE_BY_ENUM[obj->rate]             |
        ADS1X15_REG_CONFIG_CMODE_TRAD   |
        ADS1X15_REG_CONFIG_CPOL_ACTVLOW |
        ADS1X15_REG_CONFIG_CLAT_NONLAT  |
        ADS1X15_REG_CONFIG_CQUE_NONE;

    /* Operating mode */

    if (obj->mode == ADS1115_MODE_ONESHOT)
        config |= ADS1X15_REG_CONFIG_MODE_SINGLE;
    else
        config |= ADS1X15_REG_CONFIG_MODE_CONTIN;

    /* Input channel */

    config |= MUX_BY_CHANNEL[obj->current_channel];

    /* Start conversion */

    config |= ADS1X15_REG_CONFIG_OS_SINGLE;

    return config;
}



/******************************************************************************
 * @brief Starts ADS1115 conversion.
 *
 * Builds the CONFIG register and writes it to the ADS1115.
 * This function performs only hardware communication.
 *
 * @param ads Pointer to ADS1115 object.
 *
 * @return Object status.
 ******************************************************************************/
static ads1115_status_t ADS1115_StartConversion(ads1115_t *obj)
{
    uint8_t tx[3];
    uint16_t config;


    if (obj == NULL)
        return ADS1115_ERROR;

    config = ADS1115_BuildConfig(obj);

    tx[0] = ADS1X15_REG_POINTER_CONFIG;
    tx[1] = (uint8_t)(config >> 8);
    tx[2] = (uint8_t)(config);


    ads1115_driver_status_t drv_status = obj->driver->Write(obj->hw,
			obj->address, tx, sizeof(tx));

    obj->status = ADS1115_DriverStatusToStatus(drv_status);
    if(obj->status== ADS1115_ERROR){
    	obj->diagnostic.no_device++;
    }

    return obj->status;
}


static ads1115_status_t ADS1115_ReadConversion(ads1115_t *obj,
                                               uint32_t time_now)
{
    uint8_t reg;
    uint8_t buffer[2];

    uint16_t config;

    int16_t raw;


    if (obj == NULL)
        return ADS1115_INVALID_PARAMETER;

    if ((time_now - obj->conversion_start_time) < obj->min_conversion_time)
    {
    	obj->status = ADS1115_BUSY;
        return obj->status;
    }

    /*----------------------------------------------------------
     * Read CONFIG register
     *---------------------------------------------------------*/

    reg = ADS1X15_REG_POINTER_CONFIG;

	ads1115_driver_status_t drv_status = obj->driver->Write(obj->hw,
			obj->address, &reg, 1);

	obj->status = ADS1115_DriverStatusToStatus(drv_status);

    if(obj->status == ADS1115_I2C_ERROR){
    	obj->diagnostic.tx_error++;
    }

    if (obj->status != ADS1115_OK){
        return obj->status;
    }

	drv_status = obj->driver->Receive(obj->hw, obj->address, buffer, 2);

	obj->status = ADS1115_DriverStatusToStatus(drv_status);

    if(obj->status == ADS1115_I2C_ERROR){
    	obj->diagnostic.rx_error++;
        }

    if (obj->status != ADS1115_OK)
        return obj->status;

    config = ((uint16_t)buffer[0] << 8) | buffer[1];

    /*----------------------------------------------------------
     * Conversion finished?
     *---------------------------------------------------------*/

    if (ADS1115_IsBusy(config))
    {
        if ((time_now - obj->conversion_start_time) >= ADS1115_TIMEOUT_MS)
        {
            /* timeout */
        	obj->status = ADS1115_TIMEOUT;
        	obj->diagnostic.timeout++;
            return obj->status;
        }

        obj->status = ADS1115_BUSY;
        return obj->status;
    }

    /*----------------------------------------------------------
     * Read CONVERSION register
     *---------------------------------------------------------*/

    reg = ADS1X15_REG_POINTER_CONVERT;

	drv_status = obj->driver->Write(obj->hw, obj->address, &reg, 1);

	obj->status = ADS1115_DriverStatusToStatus(drv_status);

    if(obj->status == ADS1115_I2C_ERROR){
    	obj->diagnostic.tx_error++;
        }

    if(obj->status != ADS1115_OK){
        return obj->status;
    }

	drv_status = obj->driver->Receive(obj->hw, obj->address, buffer, 2);

	obj->status = ADS1115_DriverStatusToStatus(drv_status);

    if(obj->status == ADS1115_I2C_ERROR){
    	obj->diagnostic.rx_error++;
        }

    if (obj->status != ADS1115_OK)
        return obj->status;

    /*----------------------------------------------------------
     * Convert raw value
     *---------------------------------------------------------*/

    raw = (int16_t)(((uint16_t)buffer[0] << 8) | buffer[1]);

    obj->raw[obj->current_channel] = raw;


    /* convert mV */

//    ads->voltage[ads->channel] = ADS1115_RawToMilliVolts(ads, ads->raw[ads->channel]);

    obj->status = ADS1115_OK;
    return obj->status;
}

void ADS1115_Update(ads1115_t *obj, uint32_t time_now) {
	if (obj == NULL)
		return;

	switch (obj->state) {
	/*----------------------------------------------------------*/
	case ADS1115_STATE_IDLE:

		return;

		/*----------------------------------------------------------*/
	case ADS1115_STATE_START_CONVERSION:

		obj->status = ADS1115_StartConversion(obj);

		if (obj->status == ADS1115_OK) {
			obj->conversion_start_time = time_now;
			obj->status = ADS1115_BUSY;
			obj->state = ADS1115_STATE_WAIT;
		} else {
			obj->state = ADS1115_STATE_IDLE;
		}

		return;

		/*----------------------------------------------------------*/
	case ADS1115_STATE_WAIT:

		if ((time_now - obj->conversion_start_time) < obj->min_conversion_time)
			return;

		obj->state = ADS1115_STATE_RECEIVE;
		return;

		/*----------------------------------------------------------*/
	case ADS1115_STATE_RECEIVE:

		obj->status = ADS1115_ReadConversion(obj, time_now);

		if (obj->status == ADS1115_BUSY)
			return;

		if (obj->status != ADS1115_OK) {
			obj->state = ADS1115_STATE_IDLE;
			return;
		}

		obj->state = ADS1115_STATE_PROCESS;
		return;

		/*----------------------------------------------------------*/
	case ADS1115_STATE_PROCESS: {
		bool task_finished = false;

		/*----------------------------------------------------------
		 * Process conversion result
		 *---------------------------------------------------------*/

		obj->voltage[obj->current_channel] = ADS1115_RawToMilliVolts(obj,
				obj->raw[obj->current_channel]);

		obj->flags |= ADS1115_FLAG_UPDATED(obj->current_channel);

		/*----------------------------------------------------------
		 * Update task progress
		 *---------------------------------------------------------*/

		if (obj->scan == ADS1115_SCAN_ALL) {
			obj->current_channel++;

			if (obj->current_channel >= ADS1115_CHANNELS) {
				obj->current_channel = 0;

				/* Full scan completed */

				if (obj->mode == ADS1115_MODE_ONESHOT) {
					if (--obj->cycles == 0)
						task_finished = true;
				} else {
					/* Continuous + Scan All */
					task_finished = true;
				}
			}
		} else {
			/* Single channel */

			if (obj->mode == ADS1115_MODE_ONESHOT) {
				if (--obj->cycles == 0)
					task_finished = true;
			} else {
				/* Continuous + Single */
				task_finished = true;
			}
		}

		/*----------------------------------------------------------
		 * Task finished
		 *---------------------------------------------------------*/

		if (task_finished) {
			if (obj->flags & ADS1115_FLAG_STOP_REQUEST) {
				obj->flags &= ~ADS1115_FLAG_STOP_REQUEST;

				obj->status = ADS1115_IDLE;
				obj->state = ADS1115_STATE_IDLE;
			} else {
				/* Start next task */

				if ((obj->mode == ADS1115_MODE_CONTINUOUS)
						&& (obj->scan == ADS1115_SCAN_SINGLE_CHANNEL)) {
					/* ADS1115 already started next conversion */

					obj->conversion_start_time = time_now;
					obj->state = ADS1115_STATE_WAIT;
				} else {
					obj->state = ADS1115_STATE_START_CONVERSION;
				}
			}

			return;
		}

		/*----------------------------------------------------------
		 * Continue current task
		 *---------------------------------------------------------*/

		obj->state = ADS1115_STATE_START_CONVERSION;
		return;
	  }
	}
}

ads1115_status_t ADS1115_SetRate(ads1115_t *obj,
                                 ads1115_rate_t rate)
{
    if (obj == NULL)
        return ADS1115_INVALID_PARAMETER;

    if (rate > ADS1115_RATE_860SPS)
    {
    	obj->status = ADS1115_INVALID_PARAMETER;
        return obj->status;
    }

    obj->rate = rate;
    obj->min_conversion_time = CONVERSION_TIME_BY_RATE[rate];

    obj->status = ADS1115_OK;
    return obj->status;
}

ads1115_status_t ADS1115_SetGain(ads1115_t *obj,
                                 ads1115_gain_t gain)
{
    if (obj == NULL)
        return ADS1115_INVALID_PARAMETER;

    if (gain > ADS1115_GAIN_0_256V)
    {
    	obj->status = ADS1115_INVALID_PARAMETER;
        return obj->status;
    }

    obj->gain = gain;

    obj->status = ADS1115_OK;
    return obj->status;
}

int16_t ADS1115_GetRaw(const ads1115_t *obj,
                       uint8_t channel)
{
    if ((obj == NULL) || (channel >= ADS1115_CHANNELS))
        return ADS1115_ERROR;

    return obj->raw[channel];
}

int16_t ADS1115_GetVoltage(const ads1115_t *obj,
                           uint8_t channel)
{
    if ((obj == NULL) || (channel >= ADS1115_CHANNELS))
        return ADS1115_ERROR;

    return obj->voltage[channel];
}


/* Diagnostic functions */

ads1115_status_t ADS1115_GetStatus(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->status;
}

ads1115_gain_t ADS1115_GetGain(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->gain;
}

ads1115_rate_t ADS1115_GetRate(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->rate;
}

ads1115_mode_t ADS1115_GetMode(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->mode;
}

ads1115_scan_t ADS1115_GetScan(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->scan;
}

uint8_t ADS1115_GetCurrentChannel(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return obj->current_channel;
}

bool ADS1115_IsRunning(const ads1115_t *obj)
{
    if (obj == NULL)
        return ADS1115_ERROR;

    return (obj->state != ADS1115_STATE_IDLE);
}
