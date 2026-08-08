/*
 * console.c
 *
 *  Created on: Jul 31, 2026
 *      Author: jacabe
 */
#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>


static const uint8_t newline[] = "\r\n";


void Console_RxData(console_t *obj,
					const uint8_t *buf,
					uint16_t len)
{
	if ((obj == NULL) || (buf == NULL) || (len == 0))
		return;

	if (obj->rx_line_ready)
		return;

	for (uint16_t i = 0; i < len; i++)
	{
		char c = (char)buf[i];

		if ((c == '\r') || (c == '\n'))
		{
			obj->line[obj->rx_index] = '\0';
			obj->rx_index = 0;
			obj->rx_line_ready = true;
			break;
		}

		if (obj->rx_index < (CONSOLE_RX_SIZE - 1))
		{
			obj->line[obj->rx_index++] = c;
		}
	}
}



/******************************************************************************
 * @brief Prints formatted text.
 *
 * Formats a text string using a variable argument list and transmits it
 * through the console output driver.
 *
 * @param obj
 * Pointer to console object.
 *
 * @param format
 * Printf-style format string.
 *
 * @param args
 * Variable argument list.
 *
 * @return
 * Operation status.
 ******************************************************************************/
static console_status_t Console_VPrintf(
        console_t *obj,
        const char *format,
        va_list args)
{
    int len;

    if ((obj == NULL) || (format == NULL))
    {
        return CONSOLE_ERROR;
    }

    len = vsnprintf(obj->tx_printf_buffer,
                    sizeof(obj->tx_printf_buffer),
                    format,
                    args);

    if (len < 0)
    {
        return CONSOLE_ERROR;
    }

    if (len >= (int)sizeof(obj->tx_printf_buffer))
    {
        len = sizeof(obj->tx_printf_buffer) - 1;
    }

    return Console_Write(obj,
                         (const uint8_t *)obj->tx_printf_buffer,
                         (uint16_t)len);
}


console_status_t Console_PrintLine(console_t *obj, const char *format, ...)
{
    console_status_t status;
    va_list args;

    if ((obj == NULL) || (format == NULL))
    {
        return CONSOLE_ERROR;
    }

    va_start(args, format);

    status = Console_VPrintf(obj, format, args);

    va_end(args);

    if (status != CONSOLE_OK)
            return status;

    return Console_Write(obj,
                         newline,
                         sizeof(newline) - 1);

}



console_status_t Console_Printf(console_t *obj, const char *format, ...)
{
	if ((obj == NULL) || (format == NULL)){
		return CONSOLE_ERROR;
	}

	console_status_t status;

	va_list args;

	va_start(args, format);

	status = Console_VPrintf(obj, format, args);

	va_end(args);

	return status;

}

console_status_t Console_Write(console_t *obj,
							   const uint8_t *data,
							   uint16_t len)
{
	if ((obj == NULL) || (data == NULL) || (len == 0))
		return CONSOLE_ERROR;

	while (len--)
	{
		uint16_t next = obj->tx_head + 1;

		if (next >= CONSOLE_TX_SIZE)
			next = 0;

		/* FIFO pełne */
		if (next == obj->tx_tail)
		{
			if (obj->tx_overflow != UINT32_MAX)
				obj->tx_overflow++;
			return CONSOLE_OVERFLOW;
		}

		obj->tx_buffer[obj->tx_head] = *data++;
		obj->tx_head = next;
	}

	/* rozpocznij transmisję jeśli interfejs jest wolny */
	Console_TxStart(obj);

	return CONSOLE_OK;
}

void Console_TxStart(console_t *obj)
{
	if (obj == NULL)
		return;

	if (obj->tx_busy)
		return;

	if (obj->tx_head == obj->tx_tail)
		return;

	uint16_t cnt;

	if (obj->tx_head > obj->tx_tail)
	{
		/* dane są w jednym ciągłym bloku */
		cnt = obj->tx_head - obj->tx_tail;
	}
	else
	{
		/* wysyłamy do końca bufora */
		cnt = CONSOLE_TX_SIZE - obj->tx_tail;
	}

	/* rozpoczęcie transmisji */
	if ((obj->driver == NULL) ||
		(obj->driver->Write == NULL))
		return;

	if (obj->driver->Write(obj->hw,
						   (uint8_t *)&obj->tx_buffer[obj->tx_tail],
						   cnt))
	{
		obj->tx_tail += cnt;

		if (obj->tx_tail >= CONSOLE_TX_SIZE)
			obj->tx_tail = 0;

		obj->tx_busy = true;
	}
}

/*nalezy ponizsza funkcjedodac do callbackow transmisji stworzonych driverow*/

void Console_TxDone(console_t *obj)
{
	if (obj == NULL)
		return;

	obj->tx_busy = false;

	/* jeżeli są kolejne dane, wyślij je od razu */
	Console_TxStart(obj);
}

console_status_t Console_Init(console_t *obj,
							  const communication_uart_driver_t *driver,
							  void *hw)
{

	if ((obj == NULL) || (driver == NULL))
		return CONSOLE_ERROR;

	memset(obj, 0, sizeof(console_t));

	obj->driver = driver;
	obj->hw = hw;

	return CONSOLE_OK;
}



bool Console_LineReady(const console_t *obj)
{
    if (obj == NULL)
        return false;

    return obj->rx_line_ready;
}

const char *Console_GetLine(const console_t *obj)
{
    if (obj == NULL)
        return NULL;

    return obj->line;
}

void Console_LineDone(console_t *obj)
{
    if (obj == NULL)
        return;

    obj->rx_line_ready = false;
}

uint32_t Console_GetOverflow(const console_t *obj)
{
    if (obj == NULL)
        return 0;

    return obj->tx_overflow;
}

bool Console_IsBusy(const console_t *obj)
{
    if (obj == NULL)
        return false;

    return obj->tx_busy;
}
