/*
 * mcp3424.h
 *
 *  Created on: Aug 5, 2026
 *      Author: jacabe
 
 * Copyright (c) 2026 jacabe
 * Licensed under the MIT License.
 * See LICENSE file for details.

 */

#ifndef MCP3424_H_
#define MCP3424_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "communication_i2c_driver.h"

#define MCP3424_TIMEOUT_MS					270
#define MCP3424_CHANNELS					4

/* MCP3424 I2C addresses (7-bit) */

/* ADR0 = Low,   ADR1 = Low   */
#define MCP3424_ADDRESS0    0x68

/* ADR0 = Low,   ADR1 = Float */
#define MCP3424_ADDRESS1    0x69

/* ADR0 = Low,   ADR1 = High  */
#define MCP3424_ADDRESS2    0x6A

/* ADR0 = High,  ADR1 = Low   */
#define MCP3424_ADDRESS3    0x6B

/* ADR0 = High,  ADR1 = Float */
#define MCP3424_ADDRESS4    0x6C

/* ADR0 = High,  ADR1 = High  */
#define MCP3424_ADDRESS5    0x6D

/* ADR0 = Float, ADR1 = Low   */
#define MCP3424_ADDRESS6    0x6E

/* ADR0 = Float, ADR1 = High  */
#define MCP3424_ADDRESS7    0x6F

#define MCP3424_RESOLUTION_12BITS			0x0
#define MCP3424_RESOLUTION_14BITS			0x4
#define MCP3424_RESOLUTION_16BITS			0x8
#define MCP3424_RESOLUTION_18BITS			0xC

#define MCP3424_MODE_ONE_SHOT				0x0
#define MCP3424_MODE_CONTINOUS				0x10

#define MCP3424_CHANNEL0					0x0
#define MCP3424_CHANNEL1					0x20
#define MCP3424_CHANNEL2					0x40
#define MCP3424_CHANNEL3					0x60

#define MCP3424_ONE_SHOT_START_CONVERSION	0X80

#define MCP3424_CONV_TIME_12BIT_MS   5      // 4.17 ms
#define MCP3424_CONV_TIME_14BIT_MS   17     // 16.67 ms
#define MCP3424_CONV_TIME_16BIT_MS   67     // 66.67 ms
#define MCP3424_CONV_TIME_18BIT_MS   267    // 266.67 ms

#define MCP3424_LSB_12BIT_NV   1000000      // 1000 uV
#define MCP3424_LSB_14BIT_NV    250000      // 250 uV
#define MCP3424_LSB_16BIT_NV     62500      // 62.5 uV
#define MCP3424_LSB_18BIT_NV     15625      // 15.625 uV


#define MCP3424_NV_PER_MV    1000000L

#define MCP3424_GAIN_MASK         0x03
#define MCP3424_RESOLUTION_MASK   0x0C
#define MCP3424_MODE_MASK         0x10
#define MCP3424_CHANNEL_MASK      0x60
#define MCP3424_RDY_MASK          0x80


#define MCP3424_IsConversionEnd(config) \
    (((config) & MCP3424_ONE_SHOT_START_CONVERSION) == 0)


#define MCP3424_FLAG_UPDATED(ch)	 (1U << (ch))

#define MCP3424_FLAG_STOP_REQUEST    (1U << 4)


typedef enum
{
	MCP3424_RESOLUTION_12_BITS=0,
	MCP3424_RESOLUTION_14_BITS,
	MCP3424_RESOLUTION_16_BITS,
	MCP3424_RESOLUTION_18_BITS
}mcp3424_resolution_t;

typedef enum
{
	MCP3424_CH1=0,
	MCP3424_CH2,
	MCP3424_CH3,
	MCP3424_CH4
}mcp3424_channel_t;

typedef enum
{
	MCP3424_PGA_X1=0,
	MCP3424_PGA_X2,
	MCP3424_PGA_X4,
	MCP3424_PGA_X8
}mcp3424_pga_t;


typedef enum
{
    MCP3424_OK = 0,

    /* Device state */
	MCP3424_IDLE,
	MCP3424_BUSY,
	MCP3424_TIMEOUT,

    /* Communication errors */
	MCP3424_I2C_BUSY,
	MCP3424_I2C_TIMEOUT,
	MCP3424_NO_DEVICE,
	MCP3424_I2C_ERROR,

    /* API errors */
	MCP3424_INVALID_PARAMETER,
	MCP3424_INVALID_CHANNEL,

    /* Internal library error */
	MCP3424_ERROR

} mcp3424_status_t;

//typedef enum
//{
//	MCP3424_DRIVER_OK = 0,
//	MCP3424_DRIVER_BUSY,
//	MCP3424_DRIVER_ERROR
//
//} mcp3424_driver_status_t;

typedef enum{
	MCP3424_MODE_CONTINUOUS = 0,
	MCP3424_MODE_ONESHOT
}mcp3424_mode_t;

typedef enum{
	MCP3424_SCAN_ALL = 0,
	MCP3424_SCAN_SINGLE_CHANNEL
}mcp3424_scan_t;

typedef enum
{
	MCP3424_STATE_IDLE = 0,
	MCP3424_STATE_START_CONVERSION,
	MCP3424_STATE_WAIT,
	MCP3424_STATE_RECEIVE,
	MCP3424_STATE_PROCESS
} mcp3424_state_t;

typedef struct
{
    /* Communication errors */

    uint32_t no_device;
    uint32_t timeout;
    uint32_t tx_error;
    uint32_t rx_error;

    /* API errors */

    uint32_t invalid_parameter;
    uint32_t invalid_channel;

    /* Statistics */

    uint32_t start;
    uint32_t conversion;
    uint32_t stop_request;

} mcp3424_diagnostic_t;

typedef struct
{
    /* Driver */
    const communication_i2c_driver_t *driver;
    void *hw;

    /* Hardware */
    uint8_t address;

    /* Device configuration register */
    uint8_t config;

    uint8_t mode;
    uint8_t scan;

    /* State */
    uint8_t state;
    uint8_t status;
    uint8_t flags;

    /* Measurement */
    uint8_t current_channel;  /* Currently measured channel */

    uint8_t cycles;          /* 0 = continuous */

    uint32_t conversion_start_time;
    uint16_t conversion_time_ms;


    /* Results */
    int32_t raw[4];
    int32_t voltage_mv[4];
    uint32_t lsb_nv;

    /* Diagnostic counters */
    mcp3424_diagnostic_t diagnostic;

} mcp3424_t;



/**
 * @brief Initializes the MCP3424 object.
 *
 * Initializes the MCP3424 object, assigns the communication driver,
 * stores the hardware interface and I2C device address, clears the
 * internal state and prepares the object for operation.
 *
 * This function only initializes the software object.
 * It does not communicate with the MCP3424 device and does not start
 * any conversion.
 *
 * Before starting measurements, configure the desired operating
 * parameters using the available setter functions and call
 * MCP3424_Start().
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param driver
 * Pointer to the communication driver interface.
 *
 * @param hw
 * Driver-specific hardware handle.
 *
 * @param address
 * 7-bit I2C address of the MCP3424 device.
 *
 * @return
 * Current object status.
 *
 * @note
 * The communication driver is hardware dependent.
 * Example drivers for STM32 HAL are included with the framework.
 */
mcp3424_status_t MCP3424_Init(mcp3424_t *obj,
                              const communication_i2c_driver_t *driver,
                              void *hw,
                              uint8_t address);




/******************************************************************************
 * @brief Starts a new measurement task.
 *
 * Configures the MCP3424 object and starts a new asynchronous measurement
 * task.
 *
 * The function:
 * - stores the requested measurement configuration,
 * - verifies that the device is present on the I2C bus,
 * - initializes the internal state machine.
 *
 * No measurement is performed by this function.
 * The conversion process is executed by calling MCP3424_Update()
 * periodically.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param resolution
 * ADC resolution:
 * - MCP3424_RESOLUTION_12_BITS
 * - MCP3424_RESOLUTION_14_BITS
 * - MCP3424_RESOLUTION_16_BITS
 * - MCP3424_RESOLUTION_18_BITS
 *
 * @param gain
 * Programmable Gain Amplifier (PGA):
 * - MCP3424_PGA_X1
 * - MCP3424_PGA_X2
 * - MCP3424_PGA_X4
 * - MCP3424_PGA_X8
 *
 * @param mode
 * Measurement mode:
 * - MCP3424_MODE_ONESHOT
 *      Performs a finite number of conversions.
 *
 * - MCP3424_MODE_CONTINUOUS
 *      Performs continuous conversions until
 *      MCP3424_Stop() is requested.
 *
 * @param scan
 * Channel scan mode:
 *
 * - MCP3424_SCAN_SINGLE_CHANNEL
 *      Measures only the selected channel.
 *
 * - MCP3424_SCAN_ALL
 *      Measures all input channels sequentially
 *      (CH1 → CH2 → CH3 → CH4).
 *      The @p channel parameter is ignored.
 *
 * @param channel
 * Input channel number (0...3).
 * Used only when scan mode is MCP3424_SCAN_SINGLE_CHANNEL.
 *
 * @param cycles
 * Number of measurement cycles.
 *
 * Used only in MCP3424_MODE_ONESHOT:
 * - Single channel:
 *      Number of conversions.
 *
 * - Scan all:
 *      Number of complete scans through all channels.
 *
 * Ignored in MCP3424_MODE_CONTINUOUS mode.
 *
 * @param time_now
 * Current system time in milliseconds.
 * Usually HAL_GetTick().
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_Start(mcp3424_t *obj,
                               mcp3424_resolution_t resolution,
                               mcp3424_pga_t gain,
                               mcp3424_mode_t mode,
                               mcp3424_scan_t scan,
                               uint8_t channel,
                               uint8_t cycles,
                               uint32_t time_now);



/******************************************************************************
 * @brief Requests graceful termination of the current measurement task.
 *
 * Requests the currently running measurement task to stop.
 *
 * The current conversion is not aborted. If a channel scan is in progress,
 * the scan is completed before the task terminates.
 *
 * After the requested task has finished, the object returns to the
 * MCP3424_IDLE state. All previously acquired measurement results
 * remain available and can still be accessed using the getter functions.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_Stop(mcp3424_t *obj);



/******************************************************************************
 * @brief Executes one step of the MCP3424 state machine.
 *
 * Executes one iteration of the internal asynchronous state machine
 * responsible for the complete measurement process.
 *
 * Depending on the current state, the function may:
 * - start a new conversion,
 * - wait for the minimum conversion time,
 * - read the conversion result,
 * - process the acquired data,
 * - continue the measurement task or finish it.
 *
 * The function is non-blocking and should be called periodically
 * from the main application loop.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param time_now
 * Current system time in milliseconds.
 * Usually obtained from HAL_GetTick().
 ******************************************************************************/
void MCP3424_Update(mcp3424_t *obj,
                    uint32_t time_now);



/******************************************************************************
 * @brief Returns the last raw conversion result.
 *
 * Returns the most recently acquired raw ADC value for the selected channel.
 *
 * The value is updated automatically by MCP3424_Update() whenever a
 * conversion is completed.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param channel
 * Input channel number (0...3).
 *
 * @return
 * Raw ADC conversion result.
 ******************************************************************************/
int32_t MCP3424_GetRaw(const mcp3424_t *obj,
                       uint8_t channel);

/******************************************************************************
 * @brief Returns the last measured voltage.
 *
 * Returns the most recently calculated input voltage for the selected
 * channel.
 *
 * The voltage is calculated automatically by MCP3424_Update() after each
 * completed conversion and is expressed in millivolts (mV).
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param channel
 * Input channel number (0...3).
 *
 * @return
 * Measured input voltage in millivolts (mV).
 ******************************************************************************/
int32_t MCP3424_GetVoltage(const mcp3424_t *obj,
                           uint8_t channel);



/******************************************************************************
 * @brief Sets the ADC resolution.
 *
 * Configures the resolution used for subsequent conversions.
 *
 * If a measurement task is currently running, the new setting is applied
 * when the next measurement task is started.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param resolution
 * ADC resolution.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_SetResolution(mcp3424_t *obj,
                                       mcp3424_resolution_t resolution);


                                       

/******************************************************************************
 * @brief Sets the programmable gain amplifier (PGA).
 *
 * Configures the input gain used for subsequent conversions.
 *
 * If a measurement task is currently running, the new setting is applied
 * when the next measurement task is started.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param gain
 * PGA gain.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_SetGain(mcp3424_t *obj,
                                 mcp3424_pga_t gain);




/******************************************************************************
 * @brief Selects the input channel.
 *
 * Selects the input channel used in
 * MCP3424_SCAN_SINGLE_CHANNEL mode.
 *
 * If a measurement task is currently running, the new setting is applied
 * when the next measurement task is started.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param channel
 * Input channel number.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_SetChannel(mcp3424_t *obj,
                                    mcp3424_channel_t channel);




/******************************************************************************
 * @brief Sets the measurement mode.
 *
 * Configures the operating mode of the measurement task.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param mode
 * Measurement mode.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_SetMode(mcp3424_t *obj,
                                 mcp3424_mode_t mode);




/******************************************************************************
 * @brief Sets the channel scan mode.
 *
 * Selects whether a single channel or all channels are measured.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @param scan
 * Channel scan mode.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_SetScan(mcp3424_t *obj,
                                 mcp3424_scan_t scan);




/******************************************************************************
 * @brief Returns the current object status.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Current object status.
 ******************************************************************************/
mcp3424_status_t MCP3424_GetStatus(const mcp3424_t *obj);




/******************************************************************************
 * @brief Returns the configured PGA gain.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Configured PGA gain.
 ******************************************************************************/
mcp3424_pga_t MCP3424_GetGain(const mcp3424_t *obj);




/******************************************************************************
 * @brief Returns the configured ADC resolution.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Configured ADC resolution.
 ******************************************************************************/
mcp3424_resolution_t MCP3424_GetResolution(const mcp3424_t *obj);




/******************************************************************************
 * @brief Returns the configured measurement mode.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Configured measurement mode.
 ******************************************************************************/
mcp3424_mode_t MCP3424_GetMode(const mcp3424_t *obj);



/******************************************************************************
 * @brief Returns the configured channel scan mode.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Configured channel scan mode.
 ******************************************************************************/
mcp3424_scan_t MCP3424_GetScan(const mcp3424_t *obj);



/******************************************************************************
 * @brief Returns the currently active input channel.
 *
 * During channel scan mode this function returns the channel currently
 * being converted.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Current input channel.
 ******************************************************************************/
uint8_t MCP3424_GetCurrentChannel(const mcp3424_t *obj);




/******************************************************************************
 * @brief Checks whether a measurement task is running.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * true if a measurement task is active, otherwise false.
 ******************************************************************************/
bool MCP3424_IsRunning(const mcp3424_t *obj);



/******************************************************************************
 * @brief Returns diagnostic information.
 *
 * Returns a pointer to the internal diagnostic structure containing
 * communication errors and measurement statistics.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 *
 * @return
 * Pointer to the diagnostic structure.
 ******************************************************************************/
const mcp3424_diagnostic_t *MCP3424_GetDiagnostics(const mcp3424_t *obj);



/******************************************************************************
 * @brief Clears all diagnostic counters.
 *
 * Resets all communication error counters and measurement statistics.
 *
 * @param obj
 * Pointer to the MCP3424 object.
 ******************************************************************************/
void MCP3424_ResetDiagnostics(mcp3424_t *obj);

#endif /* MCP3424_H_ */
