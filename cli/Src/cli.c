/*
 * cli.c
 *
 *  Created on: Aug 2, 2026
 *      Author: jacabe
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>


#include "cli.h"
//#include "console.h"



/******************************************************************************
 * @brief Command lookup table.
 *
 * Defines all commands recognized by the CLI parser.
 *
 * Each entry contains:
 * - command string,
 * - command identifier,
 * - expected argument type.
 *
 * Commands are compared in the order they appear in the table.
 *
 * Command syntax:
 * - Commands without arguments:
 *      "save"
 *      "load"
 *
 * - Commands with integer arguments:
 *      "step:100"
 *      "engine:1"
 *
 * To add a new command, simply append a new entry to this table.
 * 
 * Example:
 *
 * Input:
 *      led:1
 *
 * Table entry:
 *      { "led:", CLI_CMD_LED, CLI_ARG_INT }
 *
 * Result:
 *      command = CLI_CMD_LED
 *      value   = 1
 *
 ******************************************************************************/
//static const cli_cmd_table_t table[] =
//{
//    { "save",    CLI_ARG_NONE,   SaveCallback,  	"Save parameters to user medium"      },
//    { "load",    CLI_ARG_NONE,   LoadCallback,		"load parameters from user medium"    },
//	{ "restore", CLI_ARG_NONE,   RestoreCallback, 	"restore parameters from user medium" }
//
//};



static const char * const cli_comments[] =
{
    [CLI_UNKNOWN_COMMAND]  = "CLI: unknown command\r\n",
    [CLI_INVALID_ARGUMENT] = "CLI: invalid argument\r\n",
    [CLI_CALLBACK_MISSING] = "CLI: callback not implemented\r\n",
    [CLI_ERROR]            = "CLI: internal error\r\n"
};



cli_status_t CLI_Init(cli_t *obj,
                      console_t *console,
                      const cli_cmd_table_t *table,
                      size_t table_size)
{
    if ((obj == NULL) ||
        (console == NULL) ||
        (table == NULL) ||
        (table_size == 0U))
    {
        return CLI_ERROR;
    }

    for (size_t i = 0; i < table_size; i++)
    {
        if (table[i].command == NULL)
        {
            return CLI_ERROR;
        }
    }


    memset(obj, 0, sizeof(cli_t));

    obj->console = console;

    obj->table = table;
    obj->table_size = table_size;

    obj->status = CLI_OK;

    return obj->status;
}



/******************************************************************************
 * @brief Copies a null-terminated string.
 *
 * Copies at most (max_len - 1) characters from the source string and
 * always terminates the destination string with a null character.
 *
 * @param dst
 * Destination buffer.
 *
 * @param src
 * Source string.
 *
 * @param max_len
 * Size of destination buffer in bytes.
 ******************************************************************************/
static bool CLI_ParseInt(const char *text,
                         cli_result_t *result)
{
    bool negative = false;
    bool digit_found = false;
    int32_t value = 0;

    if ((text == NULL) || (result == NULL))
    {
        return false;
    }

    if (*text == '-')
    {
        negative = true;
        text++;
    }

    while (*text != '\0')
    {
        if ((*text < '0') || (*text > '9'))
        {
            return false;
        }

        value = value * 10 + (*text - '0');

        digit_found = true;
        text++;
    }

    if (!digit_found)
    {
        return false;
    }

    result->value = negative ? -value : value;
    result->decimals = 0;
    result->arg = CLI_ARG_INT;
    result->valid = true;

    return true;
}


static bool CLI_ParseFloat(const char *text,
                           cli_result_t *result)
{
    bool negative = false;
    bool separator_found = false;
    bool digit_found = false;

    int32_t value = 0;
    uint8_t decimals = 0;

    if ((text == NULL) || (result == NULL))
    {
        return false;
    }

    if (*text == '-')
    {
        negative = true;
        text++;
    }

    while (*text != '\0')
    {
        if ((*text >= '0') && (*text <= '9'))
        {
            value = value * 10 + (*text - '0');

            if (separator_found)
            {
                decimals++;
            }

            digit_found = true;
        }
        else if ((*text == '.') || (*text == ','))
        {
            if (separator_found)
            {
                return false;
            }

            separator_found = true;
        }
        else
        {
            return false;
        }

        text++;
    }

    if (!digit_found)
    {
        return false;
    }

    result->value = negative ? -value : value;
    result->decimals = decimals;
    result->arg = CLI_ARG_FLOAT;
    result->valid = true;

    return true;
}

static bool CLI_ParseString(const char *text,
                            cli_result_t *result)
{
    if ((text == NULL) || (result == NULL))
    {
        return false;
    }

    strncpy(result->string,
            text,
            sizeof(result->string)-1 );

    result->string[sizeof(result->string) - 1] = '\0';

    result->decimals = 0;
    result->arg = CLI_ARG_STRING;
    result->valid = true;

    return true;
}

static const cli_cmd_table_t *CLI_FindCommand(
        const cli_t *obj,
        const char *line)
{
    if ((obj == NULL) || (line == NULL))
    {
        return NULL;
    }

    for (size_t i = 0; i < obj->table_size; i++)
    {
        const cli_cmd_table_t *cmd = &obj->table[i];

        size_t len = strlen(cmd->command);

        if (strncmp(line, cmd->command, len) == 0)
        {
            return cmd;
        }
    }

    return NULL;
}

static bool CLI_ParseArgument(
        const cli_cmd_table_t *cmd,
        const char *line,
        cli_result_t *result)
{
    if ((cmd == NULL) ||
        (line == NULL) ||
        (result == NULL))
    {
        return false;
    }

    switch (cmd->arg)
    {
        case CLI_ARG_NONE:
            result->arg = CLI_ARG_NONE;
            result->valid = true;
            return true;

        case CLI_ARG_INT:
            return CLI_ParseInt(line, result);

        case CLI_ARG_FLOAT:
            return CLI_ParseFloat(line, result);

        case CLI_ARG_STRING:
            return CLI_ParseString(line, result);

        default:
            return false;
    }
}



/******************************************************************************
 * @brief Executes command callback.
 *
 * Executes the callback associated with the parsed command.
 * If the callback is not registered, CLI_CALLBACK_MISSING is returned.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @param cmd
 * Pointer to command table entry.
 *
 * @param result
 * Pointer to parsed command result.
 *
 * @return
 * CLI_OK on success or an error status.
 ******************************************************************************/
static cli_status_t CLI_ExecuteCallback(
        cli_t *obj,
        const cli_cmd_table_t *cmd,
        const cli_result_t *result)
{
    if ((obj == NULL) || (cmd == NULL) || (result == NULL))
    {
        return CLI_ERROR;
    }

    if (cmd->callback == NULL)
    {
        obj->diagnostic.callback_missing++;
        return CLI_CALLBACK_MISSING;
    }

    cmd->callback(obj, cmd, result);

    obj->diagnostic.executed++;

    return CLI_OK;
}



/******************************************************************************
 * @brief Prints diagnostic message.
 *
 * Prints a diagnostic message corresponding to the specified CLI status.
 * Messages are printed only when enabled by commentary flags.
 *
 * @param obj
 * Pointer to CLI object.
 *
 * @param status
 * CLI status to report.
 ******************************************************************************/
static void CLI_PrintComment(cli_t *obj, cli_status_t status) {
	if ((obj == NULL) || (obj->commentary_flags == 0)) {
		return;
	}

	if ((status >= CLI_UNKNOWN_COMMAND) && (status <= CLI_ERROR)) {
		Console_Printf(obj->console, "%s", cli_comments[status]);
	}

}



static const char *CLI_GetArgumentSuffix(cli_arg_t arg)
{
    switch (arg)
    {
        case CLI_ARG_NONE:
            return "";

        case CLI_ARG_INT:
            return "<int>";

        case CLI_ARG_FLOAT:
            return "<float>";

        case CLI_ARG_STRING:
            return "<string>";

        default:
            return "";
    }
}



/******************************************************************************
 * @brief Prints command list.
 *
 * Prints all available CLI commands together with their descriptions.
 * The command list is generated automatically from the registered
 * command table.
 *
 * @param obj
 * Pointer to CLI object.
 ******************************************************************************/
 static void CLI_Help(cli_t *obj)
{
    if (obj == NULL)
    {
        return;
    }

    char cmd_text[32];

    Console_Printf(obj->console,
                   "Available commands:\r\n\r\n");

    Console_PrintLine(obj->console,
                      "%-*s %s",
                      CLI_HELP_CMD_WIDTH,
                      "help",
                      "Show this help");

    Console_PrintLine(obj->console, "\r\n");

    for (uint16_t i = 0; i < obj->table_size; i++)
    {
        const cli_cmd_table_t *cmd = &obj->table[i];
        snprintf(cmd_text,
                     sizeof(cmd_text),
                     "%s%s",
                     cmd->command,
                     CLI_GetArgumentSuffix(cmd->arg));

        Console_PrintLine(obj->console,
                          "%-*s %s",
                          CLI_HELP_CMD_WIDTH,
                          cmd_text,
                          cmd->description);
//        Console_Printf(obj->console,
//                       "%-20s%-10s %s\r\n",
//                       cmd->command,
//					   CLI_GetArgumentSuffix(cmd->arg),
//                       cmd->description);
    }

    Console_Printf(obj->console, "\r\n");
}




cli_status_t CLI_Update(cli_t *obj)
{
    const cli_cmd_table_t *cmd;
    const char *line;

    if (obj == NULL)
    {
    	obj->status = CLI_ERROR;
        return obj->status;
    }

    obj->status = CLI_OK;

    if (!Console_LineReady(obj->console))
    {
    	obj->status = CLI_NO_COMMAND;
    	return obj->status;
    }

    line = Console_GetLine(obj->console);

//    Console_Printf(obj->console,
//                   "RX: '%s'\r\n",
//                   line);

    /* built-in commands */

    if (strcmp(line, "help") == 0)
    {
        CLI_Help(obj);

//        Console_LineDone(obj->console);
        goto exit_cli_update;
    }

    /* user command */

    cmd = CLI_FindCommand(obj, line);

    if (cmd == NULL)
    {
        obj->status = CLI_UNKNOWN_COMMAND;
        obj->diagnostic.unknown_command++;

        //Console_LineDone(obj->console);
        goto exit_cli_update;
    }

    if (!CLI_ParseArgument(cmd, line + strlen(cmd->command), &obj->result))
    {
        obj->status = CLI_INVALID_ARGUMENT;
        obj->diagnostic.invalid_argument++;

//        Console_LineDone(obj->console);
        goto exit_cli_update;
    }


    /* callback */

//    if (cmd->callback == NULL)
//    {
//        obj->status = CLI_CALLBACK_MISSING;
//        obj->diagnostic.callback_missing++;
//
////        CLI_PrintComment(obj);
////
////        Console_LineDone(obj->console);
//        goto exit_cli_update;
//    }

    obj->status = CLI_ExecuteCallback(obj, cmd, &obj->result);


exit_cli_update:

    CLI_PrintComment(obj, obj->status);

    Console_LineDone(obj->console);

    return obj->status;
}






void CLI_SetCommentaryFlags(cli_t *obj,
                            uint8_t flags)
{
    if (obj == NULL)
    {
        return;
    }

    obj->commentary_flags = flags;
}


uint8_t CLI_GetCommentaryFlags(const cli_t *obj)
{
    if (obj == NULL)
    {
        return 0;
    }

    return obj->commentary_flags;
}


cli_status_t CLI_GetStatus(const cli_t *obj)
{
    if (obj == NULL)
    {
        return CLI_ERROR;
    }

    return obj->status;
}


const cli_diagnostic_t *CLI_GetDiagnostics(const cli_t *obj)
{
    if (obj == NULL)
    {
        return NULL;
    }

    return &obj->diagnostic;
}


void CLI_ResetDiagnostics(cli_t *obj)
{
    if (obj == NULL)
    {
        return;
    }

    memset(&obj->diagnostic, 0, sizeof(cli_diagnostic_t));
}
