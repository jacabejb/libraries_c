/*
 * MCP3424 example
 *
 * Continuous measurement of all four channels.
 */

#include "mcp3424.h"
#include "driver_i2c.h"

extern I2C_HandleTypeDef hi2c1;

mcp3424_t adc;

int main(void)
{
    HAL_Init();

    /* Initialize peripherals */
    MX_I2C1_Init();

    /* Initialize library */
    MCP3424_Init(&adc,
                 &COMMUNICATION_I2C,
                 &hi2c1,
                 MCP3424_ADDRESS0);

    /* Start continuous scan */
    MCP3424_Start(&adc,
                  MCP3424_RESOLUTION_18_BITS,
                  MCP3424_PGA_X1,
                  MCP3424_MODE_CONTINUOUS,
                  MCP3424_SCAN_ALL,
                  MCP3424_CH1,
                  0,
                  HAL_GetTick());

    while (1)
    {
        MCP3424_Update(&adc, HAL_GetTick());

        int32_t ch1 = MCP3424_GetVoltage(&adc, MCP3424_CH1);
        int32_t ch2 = MCP3424_GetVoltage(&adc, MCP3424_CH2);
        int32_t ch3 = MCP3424_GetVoltage(&adc, MCP3424_CH3);
        int32_t ch4 = MCP3424_GetVoltage(&adc, MCP3424_CH4);

        /* User application */

        (void)ch1;
        (void)ch2;
        (void)ch3;
        (void)ch4;
    }
}