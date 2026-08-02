/*
 * cli.c
 *
 *  Created on: Aug 2, 2026
 *      Author: jacabe
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>


#include "cli.h"
//#include "console.h"



static const cli_cmd_table_t table[] =
{
    { "save",   CLI_CMD_SAVE,   CLI_ARG_NONE },
    { "load",   CLI_CMD_LOAD,   CLI_ARG_NONE },
    { "step:",  CLI_CMD_STEP,   CLI_ARG_INT  },
    { "engine:",CLI_CMD_ENGINE, CLI_ARG_INT  },
};

#define CLI_TABLE_SIZE(x) (sizeof(x) / sizeof((x)[0]))



cli_status_t CLI_Parse(console_t *console,
                       cli_result_t *result)
{

    if ((console == NULL) || (result == NULL))
        return CLI_ERROR;

    if (!Console_LineReady(console))
        return CLI_NO_DATA;

    memset(result, 0, sizeof(cli_result_t));

    result->valid = true;

    const char *line = Console_GetLine(console);

    /* ENTER */
    if (line[0] == '\0')
    {
        result->cmd = CLI_ENTER;
        result->ready = true;

        Console_LineDone(console);

        return CLI_OK;
    }

    /* szukanie komendy */

    for (uint32_t i = 0; i < CLI_TABLE_SIZE(table); i++)
    {
        if (table[i].arg == CLI_ARG_INT)
        {
            if (strncmp(line,
                        table[i].text,
                        strlen(table[i].text)) == 0)
            {
                result->cmd = table[i].cmd;

                if (!CLI_GetInt(line + strlen(table[i].text),
                                &result->value))
                {
                    result->valid = false;
                }

                result->ready = true;

                Console_LineDone(console);

                return CLI_OK;
            }
        }
        else
        {
            if (strcmp(line,
                       table[i].text) == 0)
            {
                result->cmd = table[i].cmd;
                result->ready = true;

                Console_LineDone(console);

                return CLI_OK;
            }
        }
    }

    result->cmd   = CLI_CMD_UNKNOWN;
    result->valid = false;
    result->ready = true;

    Console_LineDone(console);

    return CLI_UNKNOWN_CMD;
}

bool CLI_GetInt(const char *text,
                int32_t *value)
{
    char *endptr;
    long val;

    if ((text == NULL) || (value == NULL))
        return false;

    /* pomiń początkowe spacje */
    while (isspace((unsigned char)*text))
        text++;

    /* pusty tekst */
    if (*text == '\0')
        return false;

    val = strtol(text, &endptr, 10);

    /* nic nie przekonwertowano */
    if (endptr == text)
        return false;

    /* pomiń końcowe spacje */
    while (isspace((unsigned char)*endptr))
        endptr++;

    /* po liczbie nie może być nic więcej */
    if (*endptr != '\0')
        return false;

    /* sprawdzenie zakresu int32_t */
    if ((val < INT32_MIN) || (val > INT32_MAX))
        return false;

    *value = (int32_t)val;

    return true;
}
