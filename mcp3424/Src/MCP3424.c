/*
 * MCP3424.C
 *
 *  Created on: Aug 5, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#include "mcp3424.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


static const uint8_t GAIN_BY_ENUM[] =
{
    MCP3424_PGA_X1,
	MCP3424_PGA_X2,
	MCP3424_PGA_X4,
	MCP3424_PGA_X8
};

static const uint8_t RESOLUTION_BY_ENUM[] = {

		MCP3424_RESOLUTION_12BITS,
		MCP3424_RESOLUTION_14BITS,
		MCP3424_RESOLUTION_16BITS,
		MCP3424_RESOLUTION_18BITS

};

static const uint16_t CONVERSION_TIME_BY_RESOLUTION[] =
{
		MCP3424_CONV_TIME_12BIT_MS,
		MCP3424_CONV_TIME_14BIT_MS,
		MCP3424_CONV_TIME_16BIT_MS,
		MCP3424_CONV_TIME_18BIT_MS
};

static const uint8_t CHANNEL_BY_ENUM[] =
{
    MCP3424_CHANNEL0,
    MCP3424_CHANNEL1,
    MCP3424_CHANNEL2,
    MCP3424_CHANNEL3
};

static const uint32_t LSB_NV_BY_RESOLUTION[] =
{
    MCP3424_LSB_12BIT_NV,
	MCP3424_LSB_14BIT_NV,
	MCP3424_LSB_16BIT_NV,
	MCP3424_LSB_18BIT_NV
};

static const uint8_t MODE_BY_ENUM[] =
{
    MCP3424_MODE_CONTINOUS,
    MCP3424_MODE_ONE_SHOT
};




mcp3424_status_t MCP3424_Init(mcp3424_t *obj, const communication_i2c_driver_t *driver,
		void *hw, uint8_t address) {


	if ((obj == NULL) || (driver == NULL)) {
		return MCP3424_ERROR;
	}

	if ((driver->IsReady == NULL) || (driver->Write == NULL) || (driver->Receive == NULL)) {
		return MCP3424_ERROR;
	}

	obj->driver  = driver;
	obj->hw      = hw;
	obj->address = address;

	obj->config = 0;

	obj->state  = MCP3424_STATE_IDLE;
	obj->status = MCP3424_IDLE;
	obj->flags  = 0;

	obj->current_channel      = 0;
	obj->cycles               = 0;
	obj->conversion_start_time = 0;
	obj->conversion_time_ms    = 0;
	obj->lsb_nv               = 0;

	memset(obj->raw, 0, sizeof(obj->raw));
	memset(obj->voltage_mv, 0, sizeof(obj->voltage_mv));
	memset(&obj->diagnostic, 0, sizeof(obj->diagnostic));

	return obj->status;
}


mcp3424_status_t MCP3424_SetResolution(mcp3424_t *obj, mcp3424_resolution_t resolution) {
	obj->config &= ~MCP3424_RESOLUTION_MASK;
	obj->config |= RESOLUTION_BY_ENUM[resolution];

	obj->conversion_time_ms = CONVERSION_TIME_BY_RESOLUTION[resolution];
	obj->lsb_nv = LSB_NV_BY_RESOLUTION[resolution];

	return MCP3424_OK;
}


mcp3424_status_t MCP3424_SetGain(mcp3424_t *obj, mcp3424_pga_t gain) {
	obj->config &= ~MCP3424_GAIN_MASK;
	obj->config |= GAIN_BY_ENUM[gain];

	return MCP3424_OK;
}


mcp3424_status_t MCP3424_SetChannel(mcp3424_t *obj, mcp3424_channel_t channel){
	obj->config &= ~MCP3424_CHANNEL_MASK;
	obj->config |= CHANNEL_BY_ENUM[channel];
	obj->current_channel = channel;

	return MCP3424_OK;
}


mcp3424_status_t MCP3424_SetMode(mcp3424_t *obj, mcp3424_mode_t mode){
	obj->config &= ~MCP3424_MODE_MASK;
	obj->config |= MODE_BY_ENUM[mode];
	obj->mode = mode;

	return MCP3424_OK;
}
mcp3424_status_t MCP3424_SetScan(mcp3424_t *obj, mcp3424_scan_t scan){
	obj->scan = scan;

	return MCP3424_OK;
}




mcp3424_status_t MCP3424_Start(mcp3424_t *obj, mcp3424_resolution_t resolution, mcp3424_pga_t gain, mcp3424_mode_t mode, mcp3424_scan_t scan,
                               uint8_t channel, uint8_t cycles, uint32_t time_now){
	if (obj == NULL)
	{
	    return MCP3424_ERROR;
	}

	if (obj->state != MCP3424_STATE_IDLE)
	{
	    return MCP3424_BUSY;
	}

	if (obj->driver->IsReady(obj->hw, obj->address) != DRIVER_OK)
	{
	    obj->diagnostic.no_device++;
	    obj->status = MCP3424_NO_DEVICE;
	    return obj->status;
	}


	MCP3424_SetResolution(obj, resolution);
	MCP3424_SetGain(obj, gain);
	MCP3424_SetChannel(obj, channel);
	MCP3424_SetMode(obj, mode);
	MCP3424_SetScan(obj, scan);

	obj->cycles = cycles;
	obj->conversion_start_time = time_now;

	obj->state = MCP3424_STATE_START_CONVERSION;

	return obj->status;
}


mcp3424_status_t MCP3424_Stop(mcp3424_t *obj)
{
    if (obj == NULL)
        return MCP3424_ERROR;

    /* Already idle */
    if (obj->state == MCP3424_STATE_IDLE)
        return obj->status;

    /* Request graceful stop */
    obj->flags |= MCP3424_FLAG_STOP_REQUEST;

    return obj->status;
}


int32_t MCP3424_GetRaw(const mcp3424_t *obj,
                       uint8_t channel)
{
	if (obj == NULL)
	    {
	        return 0;
	    }

	    if (channel > MCP3424_CH4)
	    {
	        return 0;
	    }

	return obj->raw[channel];
}

int32_t MCP3424_GetVoltage(const mcp3424_t *obj,
                           uint8_t channel){

	    if (obj == NULL)
	    {
	        return 0;
	    }

	    if (channel > MCP3424_CH4)
	    {
	        return 0;
	    }


	    return obj->voltage_mv[channel];
}

static int32_t MCP3424_RawToMilliVolts(const mcp3424_t *obj,
                                       int32_t raw)
{
	return ((raw * (int32_t)obj->lsb_nv) >> (obj->config & MCP3424_GAIN_MASK)) / MCP3424_NV_PER_MV;
}

static mcp3424_status_t MCP3424_DriverStatusToStatus(
        driver_status_t drv_status)
{
    switch (drv_status)
    {
        case DRIVER_OK:
            return MCP3424_OK;

        case DRIVER_BUSY:
            return MCP3424_BUSY;

        default:
            return MCP3424_ERROR;
    }
}

static int32_t MCP3424_ParseRaw(const uint8_t *buffer,
                                mcp3424_resolution_t resolution){

	int32_t raw = 0;

	switch(resolution){

	case MCP3424_RESOLUTION_12_BITS:
		raw = ((int16_t) buffer[0]<<8)|buffer[1];
		raw = (raw & 0x0800) ? (raw - 0x1000) : raw;
		break;

	case MCP3424_RESOLUTION_14_BITS:
		raw = ((int16_t) buffer[0] << 8) | buffer[1];
		raw = (raw & 0x2000) ? (raw - 0x4000) : raw;
		break;

	case MCP3424_RESOLUTION_16_BITS:
		raw = ((int16_t) buffer[0]<<8)|buffer[1];
		raw = (raw & 0x8000) ? (raw - 0x10000) : raw;
		break;

	case MCP3424_RESOLUTION_18_BITS:
		raw = buffer[0]<<16|buffer[1]<<8|buffer[2];
		raw = (raw & 0x20000) ? (raw - 0x40000) : raw;
		break;

	default:
		return 0;
	}

	return raw;
}

/* Diagnostic functions */

mcp3424_status_t MCP3424_GetStatus(const mcp3424_t *obj){
	if (obj == NULL)
	        return MCP3424_INVALID_PARAMETER;

	    return obj->status;
}

mcp3424_resolution_t MCP3424_GetResolution(const mcp3424_t *obj){
	if (obj == NULL){
		        return MCP3424_INVALID_PARAMETER;
	}

	return (mcp3424_resolution_t) (obj->config & MCP3424_RESOLUTION_MASK);
}

mcp3424_pga_t MCP3424_GetGain(const mcp3424_t *obj)
	{
	if (obj == NULL) {
		return MCP3424_ERROR;
	}

	return (mcp3424_pga_t) (obj->config & MCP3424_GAIN_MASK);
}

//uint16_t MCP3424_GetRate(const mcp3424_t *ads);
mcp3424_mode_t MCP3424_GetMode(const mcp3424_t *obj){
	if(obj == NULL){
		return MCP3424_ERROR;
	}

	return (mcp3424_mode_t)(obj->config &MCP3424_MODE_MASK);
}


mcp3424_scan_t MCP3424_GetScan(const mcp3424_t *obj) {
	if (obj == NULL) {
		return MCP3424_ERROR;
	}

	return obj->scan;
}


uint8_t MCP3424_GetCurrentChannel(const mcp3424_t *obj){
	if (obj == NULL) {
		return MCP3424_ERROR;
	}

	return obj->current_channel;
}


bool MCP3424_IsRunning(const mcp3424_t *obj){
	if (obj == NULL)
	        return MCP3424_ERROR;

	    return (obj->state != MCP3424_STATE_IDLE);
}


const mcp3424_diagnostic_t *MCP3424_GetDiagnostics(const mcp3424_t *obj){
	if(obj == NULL){
		return NULL;
	}

	return &obj->diagnostic;
}


void MCP3424_ResetDiagnostics(mcp3424_t *obj){
	if(obj == NULL){
			return ;
		}
	memset(&obj->diagnostic, 0, sizeof(obj->diagnostic));
}


static mcp3424_status_t MCP3424_StartConversion(mcp3424_t *obj)
{

    if (obj == NULL)
        return MCP3424_ERROR;


    driver_status_t drv_status = obj->driver->Write(
                        obj->hw,
                        obj->address,
                        &obj->config,
                        1);

    obj->status = MCP3424_DriverStatusToStatus(drv_status);
    if(obj->status== MCP3424_ERROR){
    	obj->diagnostic.no_device++;
    }

    return obj->status;
}


static mcp3424_status_t MCP3424_ReadConversion(mcp3424_t *obj,
                                               uint32_t time_now)
{
    uint8_t buffer[5];

    int32_t raw;


    if (obj == NULL)
        return MCP3424_INVALID_PARAMETER;

    if ((time_now - obj->conversion_start_time) < obj->conversion_time_ms)
    {
        obj->status = MCP3424_BUSY;
        return obj->status;
    }

    /*----------------------------------------------------------
     * Read CONFIG register
     *---------------------------------------------------------*/


    driver_status_t drv_status = obj->driver->Write(obj->hw,
                                     obj->address,
                                     &obj->config,
                                     1);

    obj->status = MCP3424_DriverStatusToStatus(drv_status);

    if(obj->status == MCP3424_I2C_ERROR){
    	obj->diagnostic.tx_error++;
    }

    if (obj->status != MCP3424_OK)
        return obj->status;

    drv_status = obj->driver->Receive(obj->hw,
                                       obj->address,
                                       buffer,
                                       5);

    obj->status = MCP3424_DriverStatusToStatus(drv_status);
    if(obj->status == MCP3424_I2C_ERROR){
        	obj->diagnostic.rx_error++;
        }

    if (obj->status != MCP3424_OK)
        return obj->status;


    /*----------------------------------------------------------
     * Conversion finished?
     *---------------------------------------------------------*/

    if (!MCP3424_IsConversionEnd(buffer[3]))
    {
        if ((time_now - obj->conversion_start_time) >= MCP3424_TIMEOUT_MS)
        {
            /* timeout */
            obj->status = MCP3424_TIMEOUT;
            obj->diagnostic.timeout++;
            return obj->status;
        }

        obj->status = MCP3424_BUSY;
        return obj->status;
    }

    /*----------------------------------------------------------
     * Convert raw value
     *---------------------------------------------------------*/

    raw = MCP3424_ParseRaw(buffer, MCP3424_GetResolution(obj));

    obj->raw[obj->current_channel] = raw;


    /* convert mV */

    obj->voltage_mv[obj->current_channel] = MCP3424_RawToMilliVolts(obj, obj->raw[obj->current_channel]);

    obj->status = MCP3424_OK;
    return obj->status;
}


void MCP3424_Update(mcp3424_t *obj, uint32_t time_now) {
	if (obj == NULL)
		return;

	switch (obj->state) {
	/*----------------------------------------------------------*/
	case MCP3424_STATE_IDLE:

		return;

		/*----------------------------------------------------------*/
	case MCP3424_STATE_START_CONVERSION:

		obj->status = MCP3424_StartConversion(obj);

		if (obj->status == MCP3424_OK) {
			obj->conversion_start_time = time_now;
			obj->status = MCP3424_BUSY;
			obj->state = MCP3424_STATE_WAIT;
		} else {
			obj->state = MCP3424_STATE_IDLE;
		}

		return;

		/*----------------------------------------------------------*/
	case MCP3424_STATE_WAIT:

		if ((time_now - obj->conversion_start_time) < obj->conversion_time_ms)
			return;

		obj->state = MCP3424_STATE_RECEIVE;
		return;

		/*----------------------------------------------------------*/
	case MCP3424_STATE_RECEIVE:

		obj->status = MCP3424_ReadConversion(obj, time_now);

		if (obj->status == MCP3424_BUSY)
			return;

		if (obj->status != MCP3424_OK) {
			obj->state = MCP3424_STATE_IDLE;
			return;
		}

		obj->state = MCP3424_STATE_PROCESS;
		return;

		/*----------------------------------------------------------*/
	case MCP3424_STATE_PROCESS: {
		bool task_finished = false;

		/*----------------------------------------------------------
		 * Process conversion result
		 *---------------------------------------------------------*/

		obj->voltage_mv[obj->current_channel] = MCP3424_RawToMilliVolts(obj,
				obj->raw[obj->current_channel]);

		obj->flags |= MCP3424_FLAG_UPDATED(obj->current_channel);

		/*----------------------------------------------------------
		 * Update task progress
		 *---------------------------------------------------------*/

		if (obj->scan == MCP3424_SCAN_ALL) {
			obj->current_channel++;

			if (obj->current_channel >= MCP3424_CHANNELS) {
				obj->current_channel = 0;

				/* Full scan completed */

				if (obj->mode == MCP3424_MODE_ONESHOT) {
					if (--obj->cycles == 0)
						task_finished = true;
				} else {
					/* Continuous + Scan All */
					task_finished = true;
				}
			}
		} else {
			/* Single channel */

			if (obj->mode == MCP3424_MODE_ONESHOT) {
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
			if (obj->flags & MCP3424_FLAG_STOP_REQUEST) {
				obj->flags &= ~MCP3424_FLAG_STOP_REQUEST;

				obj->status = MCP3424_IDLE;
				obj->state = MCP3424_STATE_IDLE;
			} else {
				/* Start next task */

				if ((obj->mode == MCP3424_MODE_CONTINUOUS)
						&& (obj->scan == MCP3424_SCAN_SINGLE_CHANNEL)) {
					/* ADS1115 already started next conversion */

					obj->conversion_start_time = time_now;
					obj->state = MCP3424_STATE_WAIT;
				} else {
					obj->state = MCP3424_STATE_START_CONVERSION;
				}
			}

			return;
		}

		/*----------------------------------------------------------
		 * Continue current task
		 *---------------------------------------------------------*/

		obj->state = MCP3424_STATE_START_CONVERSION;
		return;
	  }
	}
}
