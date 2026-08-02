/*
 * cli.h
 *
 *  Created on: Aug 2, 2026
 *      Author: jacabe
 */

#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stdint.h>

#include "console.h"

typedef enum
{
    CLI_OK = 0,
	CLI_ENTER,
    CLI_UNKNOWN_CMD,
    CLI_INVALID_ARG,
    CLI_ERROR,
	CLI_NO_DATA
} cli_status_t;

typedef enum
{
	CLI_ARG_NONE,
	CLI_ARG_INT,
	CLI_ARG_STRING
} cli_arg_t;

typedef enum
{
    CLI_CMD_NONE = 0,
	CLI_CMD_ENTER,
	CLI_CMD_E,
	CLI_CMD_TCAL,
	CLI_CMD_T_WEIGHT,
	CLI_CMD_SAVE,
	CLI_CMD_LOAD,
	CLI_CMD_STEP,
	CLI_CMD_ENGINE,

	CLI_CMD_UNKNOWN
} cli_cmd_t;


typedef struct
{
    const char *text;
    cli_cmd_t cmd;
    cli_arg_t arg;
} cli_cmd_table_t;


typedef struct
{
    bool ready;

    cli_cmd_t cmd;

    int32_t value;

    char string[32];

    bool valid;

} cli_result_t;

cli_status_t CLI_Parse(console_t *console, cli_result_t *result);

bool CLI_GetInt(const char *text, int32_t *value);

#endif /* CLI_H_ */
