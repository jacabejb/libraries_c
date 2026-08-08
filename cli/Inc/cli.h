/*
 * cli.h
 *
 *  Created on: Aug 2, 2026
 *      Author: jacabe
 */

#ifndef CLI_H_
#define CLI_H_


#include <stdint.h>
#include <stddef.h>

#include "console.h"

#define CLI_TABLE_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define CLI_HELP_CMD_WIDTH    30U


typedef struct cli_t cli_t;
typedef struct cli_cmd_table_t cli_cmd_table_t;
typedef struct cli_result_t cli_result_t;


typedef enum
{
	CLI_ARG_NONE,
	CLI_ARG_INT,
	CLI_ARG_FLOAT,
	CLI_ARG_STRING
} cli_arg_t;

typedef enum
{
    CLI_OK = 0,

    CLI_NO_COMMAND,

    CLI_UNKNOWN_COMMAND,

    CLI_INVALID_ARGUMENT,

    CLI_CALLBACK_MISSING,

    CLI_ERROR

} cli_status_t;

typedef enum
{
    CLI_COMMENT_UNKNOWN_COMMAND  = (1U << 0),

    CLI_COMMENT_INVALID_ARGUMENT = (1U << 1),

    CLI_COMMENT_CALLBACK_MISSING = (1U << 2),

    CLI_COMMENT_OK               = (1U << 3)

} cli_comment_t;


typedef cli_status_t(*cli_callback_t)(cli_t *obj, const cli_cmd_table_t *cmd, const cli_result_t *result);



typedef struct cli_result_t
{
    int32_t value;

    uint8_t decimals;

    char string[32];

    bool valid;

    cli_arg_t arg;

} cli_result_t;



typedef struct cli_cmd_table_t
{
    const char      *command;

    cli_arg_t        arg;

    cli_callback_t   callback;

    const char *description;

} cli_cmd_table_t;



typedef struct
{
    uint32_t commands;

    uint32_t executed;

    uint32_t unknown_command;

    uint32_t invalid_argument;

    uint32_t callback_missing;

} cli_diagnostic_t;


typedef struct cli_t
{
    console_t *console;

    const cli_cmd_table_t *table;

    uint16_t table_size;

    cli_status_t status;

    cli_result_t result;

    cli_diagnostic_t diagnostic;

    uint8_t commentary_flags;

} cli_t;



/******************************************************************************
 * @brief Initializes CLI library object.
 *
 * Initializes the CLI object, assigns the console interface and command
 * table, clears the internal state and prepares the parser for operation.
 *
 * This function does not parse any commands and does not execute any
 * callbacks.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @param console
 * Pointer to initialized Console object.
 *
 * @param table
 * Pointer to command table.
 *
 * @param table_size
 * Number of entries in the command table.
 *
 * @return
 * CLI_OK on success or an error code.
 ******************************************************************************/
cli_status_t CLI_Init(cli_t *obj,
                      console_t *console,
                      const cli_cmd_table_t *table,
					  size_t table_size);




/******************************************************************************
 * @brief Processes CLI commands.
 *
 * Checks whether a complete command line has been received, parses the
 * command and its argument, executes the associated callback function and
 * updates the CLI status.
 *
 * This function should be called periodically from the main loop or a
 * dedicated task.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @return
 * Current operation status.
 ******************************************************************************/
cli_status_t CLI_Update(cli_t *obj);



/******************************************************************************
 * @brief Returns current CLI status.
 *
 * Returns the status generated during the last call to CLI_Update().
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @return
 * Current CLI status.
 ******************************************************************************/
cli_status_t CLI_GetStatus(const cli_t *obj);



/*   DIAGNOSTICS   */

/******************************************************************************
 * @brief Sets CLI commentary flags.
 *
 * Enables or disables informational messages printed to the associated
 * console during command processing.
 *
 * Multiple flags may be combined using the bitwise OR operator.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @param flags
 * Commentary flags.
 ******************************************************************************/
void CLI_SetCommentaryFlags(cli_t *obj,
                            uint8_t flags);



/******************************************************************************
 * @brief Returns commentary flags.
 *
 * Returns the current commentary configuration flags used to control
 * diagnostic messages printed by the CLI.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @return
 * Current commentary flags.
 ******************************************************************************/
uint8_t CLI_GetCommentaryFlags(const cli_t *obj);


/******************************************************************************
 * @brief Returns current CLI status.
 *
 * Returns the status generated during the last call to CLI_Update().
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @return
 * Current object status.
 ******************************************************************************/
cli_status_t CLI_GetStatus(const cli_t *obj);



/******************************************************************************
 * @brief Returns pointer to diagnostic information.
 *
 * Returns a pointer to the internal diagnostic structure containing
 * command processing statistics.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @return
 * Pointer to diagnostic structure.
 ******************************************************************************/
const cli_diagnostic_t *CLI_GetDiagnostics(const cli_t *obj);



/******************************************************************************
 * @brief Clears all diagnostic counters.
 *
 * Resets all diagnostic information collected by the CLI library.
 *
 * @param obj
 * Pointer to CLI object.
 ******************************************************************************/
void CLI_ResetDiagnostics(cli_t *obj);



/******************************************************************************/



#endif /* CLI_H_ */
