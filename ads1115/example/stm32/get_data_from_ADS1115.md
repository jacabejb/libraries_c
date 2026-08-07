#include "ads1115.h"
#include "ads1115_driver_i2c.h"

/* STM32 HAL I2C handle */
extern I2C_HandleTypeDef hi2c1;

/* One complete scan of all ADS1115 channels */
typedef struct
{
    int16_t raw[ADS1115_CHANNELS];       /* Raw ADC values */
    int16_t voltage[ADS1115_CHANNELS];   /* Converted values in mV */

} ads1115_measurement_t;

/* ADS1115 library object */
ads1115_t ads;

/* Buffer for 10 complete scans */
ads1115_measurement_t measurement[10];

/* Number of completed scans stored in the buffer */
uint8_t measurement_cnt = 0;

int main(void)
{
    HAL_Init();

    /* Initialize clocks and peripherals */
    ...

    /*----------------------------------------------------------
     * Initialize ADS1115 object
     *---------------------------------------------------------*/
    ADS1115_Init(&ads,                  /* Library object             */
                 &ADS1115_Driver_I2C,   /* Communication driver        */
                 &hi2c1,                /* HAL I2C handle             */
                 ADS1X15_ADDRESS_GND);  /* ADS1115 I2C address        */

    /* Set input voltage range to ±4.096 V */
    ADS1115_SetGain(&ads, ADS1115_GAIN_4_096V);

    /* Set conversion speed to 128 samples/s */
    ADS1115_SetRate(&ads, ADS1115_RATE_128SPS);

    /*----------------------------------------------------------
     * Start measurement task
     *---------------------------------------------------------
     *
     * Mode    : OneShot
     * Scan    : All channels
     * Channel : Ignored in Scan All mode
     * Cycles  : Perform 10 complete scans
     *
     * Scan sequence:
     *
     * CH0 -> CH1 -> CH2 -> CH3
     * CH0 -> CH1 -> CH2 -> CH3
     * ...
     * (10 times)
     */
    ADS1115_Start(&ads,
                  ADS1115_MODE_ONESHOT,
                  ADS1115_SCAN_ALL,
                  0,                  /* Ignored */
                  10,                 /* Number of complete scans */
                  HAL_GetTick());

    while (1)
    {
        /*------------------------------------------------------
         * Execute one step of the ADS1115 state machine.
         *
         * This function never blocks.
         * It should be called as often as possible.
         *-----------------------------------------------------*/
        ADS1115_Update(&ads, HAL_GetTick());

        /*------------------------------------------------------
         * Stop when the measurement task has finished.
         *-----------------------------------------------------*/
        if (!ADS1115_IsRunning(&ads))
            break;

        /*------------------------------------------------------
         * A complete scan is ready only when ALL four channels
         * have been updated.
         *-----------------------------------------------------*/
        if (ADS1115_IsUpdated(&ads, 0) &&
            ADS1115_IsUpdated(&ads, 1) &&
            ADS1115_IsUpdated(&ads, 2) &&
            ADS1115_IsUpdated(&ads, 3))
        {
            /* Copy one complete scan to user buffer */

            for (uint8_t ch = 0; ch < ADS1115_CHANNELS; ch++)
            {
                measurement[measurement_cnt].raw[ch] =
                    ADS1115_GetRaw(&ads, ch);

                measurement[measurement_cnt].voltage[ch] =
                    ADS1115_GetVoltage(&ads, ch);

                /* Clear update flag for this channel */
                ADS1115_ClearUpdated(&ads, ch);
            }

            /* Next scan will be stored here */
            measurement_cnt++;
        }
    }

    /*----------------------------------------------------------
     * Measurement finished.
     *
     * measurement[0] ... first complete scan
     * measurement[1] ... second complete scan
     * ...
     * measurement[9] ... tenth complete scan
     *---------------------------------------------------------*/

    while (1)
    {
    }
}
