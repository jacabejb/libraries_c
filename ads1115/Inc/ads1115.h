/*
 * ads1115.h
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



#ifndef INC_ADS1115_H_
#define INC_ADS1115_H_

#include <communication_i2c_driver.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define ADS1X15_ADDRESS_GND (0x48) ///< 1001 000 (ADDR = GND)
#define ADS1X15_ADDRESS_VCC (0x49) ///< 1001 000 (ADDR = VCC)
#define ADS1X15_ADDRESS_SDA (0x4A) ///< 1001 000 (ADDR = SDA)
#define ADS1X15_ADDRESS_SCL (0x4B) ///< 1001 000 (ADDR = SCL)
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
#define ADS1X15_REG_POINTER_MASK (0x03)      ///< Point mask
#define ADS1X15_REG_POINTER_CONVERT (0x00)   ///< Conversion
#define ADS1X15_REG_POINTER_CONFIG (0x01)    ///< Configuration
#define ADS1X15_REG_POINTER_LOWTHRESH (0x02) ///< Low threshold
#define ADS1X15_REG_POINTER_HITHRESH (0x03)  ///< High threshold
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
#define ADS1X15_REG_CONFIG_OS_MASK (0x8000) ///< OS Mask
#define ADS1X15_REG_CONFIG_OS_SINGLE                                           \
  (0x8000) ///< Write: Set to start a single-conversion
#define ADS1X15_REG_CONFIG_OS_BUSY                                             \
  (0x0000) ///< Read: Bit = 0 when conversion is in progress
#define ADS1X15_REG_CONFIG_OS_NOTBUSY                                          \
  (0x8000) ///< Read: Bit = 1 when device is not performing a conversion

#define ADS1X15_REG_CONFIG_MUX_MASK (0x7000) ///< Mux Mask
#define ADS1X15_REG_CONFIG_MUX_DIFF_0_1                                        \
  (0x0000) ///< Differential P = AIN0, N = AIN1 (default)
#define ADS1X15_REG_CONFIG_MUX_DIFF_0_3                                        \
  (0x1000) ///< Differential P = AIN0, N = AIN3
#define ADS1X15_REG_CONFIG_MUX_DIFF_1_3                                        \
  (0x2000) ///< Differential P = AIN1, N = AIN3
#define ADS1X15_REG_CONFIG_MUX_DIFF_2_3                                        \
  (0x3000) ///< Differential P = AIN2, N = AIN3
#define ADS1X15_REG_CONFIG_MUX_SINGLE_0 (0x4000) ///< Single-ended AIN0
#define ADS1X15_REG_CONFIG_MUX_SINGLE_1 (0x5000) ///< Single-ended AIN1
#define ADS1X15_REG_CONFIG_MUX_SINGLE_2 (0x6000) ///< Single-ended AIN2
#define ADS1X15_REG_CONFIG_MUX_SINGLE_3 (0x7000) ///< Single-ended AIN3

#define ADS1X15_REG_CONFIG_PGA_MASK (0x0E00)   ///< PGA Mask
#define ADS1X15_REG_CONFIG_PGA_6_144V (0x0000) ///< +/-6.144V range = Gain 2/3
#define ADS1X15_REG_CONFIG_PGA_4_096V (0x0200) ///< +/-4.096V range = Gain 1
#define ADS1X15_REG_CONFIG_PGA_2_048V                                          \
  (0x0400) ///< +/-2.048V range = Gain 2 (default)
#define ADS1X15_REG_CONFIG_PGA_1_024V (0x0600) ///< +/-1.024V range = Gain 4
#define ADS1X15_REG_CONFIG_PGA_0_512V (0x0800) ///< +/-0.512V range = Gain 8
#define ADS1X15_REG_CONFIG_PGA_0_256V (0x0A00) ///< +/-0.256V range = Gain 16

#define ADS1X15_REG_CONFIG_MODE_MASK (0x0100)   ///< Mode Mask
#define ADS1X15_REG_CONFIG_MODE_CONTIN (0x0000) ///< Continuous conversion mode
#define ADS1X15_REG_CONFIG_MODE_SINGLE                                         \
  (0x0100) ///< Power-down single-shot mode (default)

#define ADS1X15_REG_CONFIG_RATE_MASK (0x00E0) ///< Data Rate Mask

#define ADS1X15_REG_CONFIG_CMODE_MASK (0x0010) ///< CMode Mask
#define ADS1X15_REG_CONFIG_CMODE_TRAD                                          \
  (0x0000) ///< Traditional comparator with hysteresis (default)
#define ADS1X15_REG_CONFIG_CMODE_WINDOW (0x0010) ///< Window comparator

#define ADS1X15_REG_CONFIG_CPOL_MASK (0x0008) ///< CPol Mask
#define ADS1X15_REG_CONFIG_CPOL_ACTVLOW                                        \
  (0x0000) ///< ALERT/RDY pin is low when active (default)
#define ADS1X15_REG_CONFIG_CPOL_ACTVHI                                         \
  (0x0008) ///< ALERT/RDY pin is high when active

#define ADS1X15_REG_CONFIG_CLAT_MASK                                           \
  (0x0004) ///< Determines if ALERT/RDY pin latches once asserted
#define ADS1X15_REG_CONFIG_CLAT_NONLAT                                         \
  (0x0000) ///< Non-latching comparator (default)
#define ADS1X15_REG_CONFIG_CLAT_LATCH (0x0004) ///< Latching comparator

#define ADS1X15_REG_CONFIG_CQUE_MASK (0x0003) ///< CQue Mask
#define ADS1X15_REG_CONFIG_CQUE_1CONV                                          \
  (0x0000) ///< Assert ALERT/RDY after one conversions
#define ADS1X15_REG_CONFIG_CQUE_2CONV                                          \
  (0x0001) ///< Assert ALERT/RDY after two conversions
#define ADS1X15_REG_CONFIG_CQUE_4CONV                                          \
  (0x0002) ///< Assert ALERT/RDY after four conversions
#define ADS1X15_REG_CONFIG_CQUE_NONE                                           \
  (0x0003) ///< Disable the comparator and put ALERT/RDY in high state (default)

/**************************************************************************/
#define ADS1115_ADC_FULL_SCALE		32768

#define ADS1115_TIMEOUT_MS 			  150

#define ADS1115_CHANNELS 			    4


#define ADS1115_IsBusy(config) 		(((config) & ADS1X15_REG_CONFIG_OS_MASK) == 0)


#define ADS1115_FLAG_UPDATED(ch)	 (1U << (ch))

#define ADS1115_FLAG_STOP_REQUEST    (1U << 4)


/** Gain settings */
typedef enum
{
    ADS1115_GAIN_6_144V = 0,
    ADS1115_GAIN_4_096V,
    ADS1115_GAIN_2_048V,
    ADS1115_GAIN_1_024V,
    ADS1115_GAIN_0_512V,
    ADS1115_GAIN_0_256V

} ads1115_gain_t;

/** Rate SPS */
typedef enum
{
    ADS1115_RATE_8SPS = 0,
    ADS1115_RATE_16SPS,
    ADS1115_RATE_32SPS,
    ADS1115_RATE_64SPS,
    ADS1115_RATE_128SPS,
    ADS1115_RATE_250SPS,
    ADS1115_RATE_475SPS,
    ADS1115_RATE_860SPS

} ads1115_rate_t;

typedef enum
{
    ADS1115_OK = 0,

    /* Device state */
	ADS1115_IDLE,
    ADS1115_BUSY,
	ADS1115_TIMEOUT,

    /* Communication errors */
    ADS1115_I2C_BUSY,
    ADS1115_I2C_TIMEOUT,
    ADS1115_NO_DEVICE,
    ADS1115_I2C_ERROR,

    /* API errors */
    ADS1115_INVALID_PARAMETER,
    ADS1115_INVALID_CHANNEL,

    /* Internal library error */
    ADS1115_ERROR

} ads1115_status_t;

typedef enum
{
    ADS1115_DRIVER_OK = 0,
    ADS1115_DRIVER_BUSY,
    ADS1115_DRIVER_ERROR

} ads1115_driver_status_t;

typedef enum{
	ADS1115_MODE_CONTINUOUS = 0,
	ADS1115_MODE_ONESHOT
}ads1115_mode_t;

typedef enum{
	ADS1115_SCAN_ALL = 0,
	ADS1115_SCAN_SINGLE_CHANNEL
}ads1115_scan_t;

typedef enum
{
    ADS1115_STATE_IDLE = 0,
	ADS1115_STATE_START_CONVERSION,
    ADS1115_STATE_WAIT,
    ADS1115_STATE_RECEIVE,
	ADS1115_STATE_PROCESS
} ads1115_state_t;

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

} ads1115_diagnostic_t;

typedef struct
{
    /* Driver */
    const communication_i2c_driver_t *driver;
    void *hw;

    /* Hardware */
    uint8_t address;

    /* Configuration */
    uint8_t gain;
    uint8_t rate;

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
    uint16_t min_conversion_time;

    /* Device configuration */
    uint16_t config;

    /* Results */
    int16_t raw[4];
    int16_t voltage[4];

    /* Diagnostic counters */
    ads1115_diagnostic_t diagnostic;

} ads1115_t;



/******************************************************************************
 * @brief Initializes the ADS1115 object.
 *
 * Initializes the ADS1115 object, assigns the communication driver,
 * stores the hardware interface and I2C device address, clears the
 * internal state and prepares the object for operation.
 *
 * This function only initializes the software object.
 * It does not communicate with the ADS1115 device and does not start
 * any conversion.
 *
 * Before starting measurements, configure the desired operating
 * parameters using the available setter functions and call
 * ADS1115_Start().
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param driver
 * Pointer to the communication driver interface.
 *
 * @param hw
 * Driver-specific hardware handle.
 *
 * @param address
 * 7-bit I2C address of the ADS1115 device.
 *
 * @return
 * Current object status.
 *
 * @note
 * The communication driver is hardware dependent.
 * Example drivers for STM32 HAL are included with the framework.
 ******************************************************************************/
ads1115_status_t ADS1115_Init(ads1115_t *obj,
                              const communication_i2c_driver_t *driver,
                              void *hw,
                              uint8_t address);



/******************************************************************************
 * @brief Sets the programmable gain amplifier (PGA).
 *
 * Configures the input gain used for subsequent conversions.
 *
 * If a measurement task is currently running, the new setting is applied
 * when the next measurement task is started.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param gain
 * PGA gain.
 *
 * @return
 * Current object status.
 ******************************************************************************/
ads1115_status_t ADS1115_SetGain(ads1115_t *obj,
                                 ads1115_gain_t gain);


/******************************************************************************
 * @brief Sets the ADC conversion rate.
 *
 * Configures the conversion rate used for subsequent measurements.
 *
 * If a measurement task is currently running, the new setting is applied
 * when the next measurement task is started.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param rate
 * ADC conversion rate.
 *
 * @return
 * Current object status.
 ******************************************************************************/
ads1115_status_t ADS1115_SetRate(ads1115_t *obj,
                                 ads1115_rate_t rate);


/******************************************************************************
 * @brief Starts a new measurement task.
 *
 * Configures the ADS1115 object and starts a new asynchronous measurement
 * task.
 *
 * The function:
 * - stores the requested measurement configuration,
 * - verifies that the device is present on the I2C bus,
 * - initializes the internal state machine.
 *
 * No measurement is performed by this function.
 * The conversion process is executed by calling ADS1115_Update()
 * periodically.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param mode
 * Measurement mode:
 * - ADS1115_MODE_ONESHOT
 * - ADS1115_MODE_CONTINUOUS
 *
 * @param scan
 * Channel scan mode:
 * - ADS1115_SCAN_SINGLE_CHANNEL
 * - ADS1115_SCAN_ALL
 *
 * @param channel
 * Input channel number (0...3).
 * Used only when scan mode is ADS1115_SCAN_SINGLE_CHANNEL.
 *
 * @param cycles
 * Number of measurement cycles.
 * Ignored in ADS1115_MODE_CONTINUOUS mode.
 *
 * @param time_now
 * Current system time in milliseconds.
 * Usually obtained from HAL_GetTick().
 *
 * @return
 * Current object status.
 ******************************************************************************/
ads1115_status_t ADS1115_Start(ads1115_t *obj,
                               ads1115_mode_t mode,
                               ads1115_scan_t scan,
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
 * ADS1115_IDLE state. All previously acquired measurement results
 * remain available and can still be accessed using the getter functions.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @return
 * Current object status.
 ******************************************************************************/
ads1115_status_t ADS1115_Stop(ads1115_t *obj);




/******************************************************************************
 * @brief Executes one step of the ADS1115 state machine.
 *
 * Executes one iteration of the internal asynchronous state machine
 * responsible for the complete measurement process.
 *
 * Depending on the current state, the function may:
 * - start a new conversion,
 * - wait for conversion completion,
 * - read the conversion result,
 * - process the acquired data,
 * - continue the measurement task or finish it.
 *
 * The function is non-blocking and should be called periodically
 * from the main application loop.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param time_now
 * Current system time in milliseconds.
 * Usually obtained from HAL_GetTick().
 ******************************************************************************/
void ADS1115_Update(ads1115_t *obj,
                    uint32_t time_now);



/******************************************************************************
 * @brief Returns the last raw conversion result.
 *
 * Returns the most recently acquired raw ADC value for the selected channel.
 *
 * The value is updated automatically by ADS1115_Update() whenever a
 * conversion is completed.
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param channel
 * Input channel number (0...3).
 *
 * @return
 * Raw ADC conversion result.
 ******************************************************************************/
int16_t ADS1115_GetRaw(const ads1115_t *obj,
                       uint8_t channel);



/******************************************************************************
 * @brief Returns the last measured voltage.
 *
 * Returns the most recently calculated input voltage for the selected
 * channel.
 *
 * The voltage is calculated automatically by ADS1115_Update() after each
 * completed conversion and is expressed in millivolts (mV).
 *
 * @param obj
 * Pointer to the ADS1115 object.
 *
 * @param channel
 * Input channel number (0...3).
 *
 * @return
 * Measured input voltage in millivolts (mV).
 ******************************************************************************/
int16_t ADS1115_GetVoltage(const ads1115_t *obj,
                           uint8_t channel);



/* Diagnostic functions */

/******************************************************************************
 * @brief Returns the current object status.
 ******************************************************************************/
ads1115_status_t ADS1115_GetStatus(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns the configured PGA gain.
 ******************************************************************************/
ads1115_gain_t ADS1115_GetGain(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns the configured ADC conversion rate.
 ******************************************************************************/
ads1115_rate_t ADS1115_GetRate(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns the configured measurement mode.
 ******************************************************************************/
ads1115_mode_t ADS1115_GetMode(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns the configured channel scan mode.
 ******************************************************************************/
ads1115_scan_t ADS1115_GetScan(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns the currently active input channel.
 *
 * During channel scan mode this function returns the channel currently
 * being converted.
 ******************************************************************************/
uint8_t ADS1115_GetCurrentChannel(const ads1115_t *obj);



/******************************************************************************
 * @brief Checks whether a measurement task is running.
 *
 * @return
 * true if a measurement task is active, otherwise false.
 ******************************************************************************/
bool ADS1115_IsRunning(const ads1115_t *obj);



/******************************************************************************
 * @brief Returns diagnostic information.
 *
 * Returns a pointer to the internal diagnostic structure containing
 * communication errors and measurement statistics.
 ******************************************************************************/
const ads1115_diagnostic_t *ADS1115_GetDiagnostics(const ads1115_t *obj);



/******************************************************************************
 * @brief Clears all diagnostic counters.
 *
 * Resets all communication error counters and measurement statistics.
 ******************************************************************************/
void ADS1115_ResetDiagnostics(ads1115_t *obj);

#endif /* INC_ADS1115_H_ */
