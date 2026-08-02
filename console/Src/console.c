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




void Console_RxData(console_t *con,
					const uint8_t *buf,
					uint16_t len)
{
	if ((con == NULL) || (buf == NULL) || (len == 0))
		return;

	if (con->rx_line_ready)
		return;

	for (uint16_t i = 0; i < len; i++)
	{
		char c = (char)buf[i];

		if ((c == '\r') || (c == '\n'))
		{
			con->line[con->rx_index] = '\0';
			con->rx_index = 0;
			con->rx_line_ready = true;
			break;
		}

		if (con->rx_index < (CONSOLE_RX_SIZE - 1))
		{
			con->line[con->rx_index++] = c;
		}
	}
}

void Console_Printf(console_t *con, const char *fmt, ...)
{
	if ((con == NULL) || (fmt == NULL))
		return;

	char buffer[CONSOLE_PRINTF_BUFFER_SIZE];

	va_list args;

	va_start(args, fmt);

	int len = vsnprintf(buffer,
						sizeof(buffer),
						fmt,
						args);

	va_end(args);

	if (len <= 0)
		return;

	if (len >= sizeof(buffer))
		len = sizeof(buffer) - 1;

	Console_Write(con,
				  (const uint8_t *)buffer,
				  (uint16_t)len);
}

console_status_t Console_Write(console_t *con,
							   const uint8_t *data,
							   uint16_t len)
{
	if ((con == NULL) || (data == NULL) || (len == 0))
		return CONSOLE_ERROR;

	while (len--)
	{
		uint16_t next = con->tx_head + 1;

		if (next >= CONSOLE_TX_SIZE)
			next = 0;

		/* FIFO pełne */
		if (next == con->tx_tail)
		{
			if (con->tx_overflow != UINT32_MAX)
				con->tx_overflow++;
			return CONSOLE_OVERFLOW;
		}

		con->tx_buffer[con->tx_head] = *data++;
		con->tx_head = next;
	}

	/* rozpocznij transmisję jeśli interfejs jest wolny */
	Console_TxStart(con);

	return CONSOLE_OK;
}

void Console_TxStart(console_t *con)
{
	if (con == NULL)
		return;

	if (con->tx_busy)
		return;

	if (con->tx_head == con->tx_tail)
		return;

	uint16_t cnt;

	if (con->tx_head > con->tx_tail)
	{
		/* dane są w jednym ciągłym bloku */
		cnt = con->tx_head - con->tx_tail;
	}
	else
	{
		/* wysyłamy do końca bufora */
		cnt = CONSOLE_TX_SIZE - con->tx_tail;
	}

	/* rozpoczęcie transmisji */
	if ((con->driver == NULL) ||
		(con->driver->Write == NULL))
		return;

	if (con->driver->Write(con->hw,
						   (uint8_t *)&con->tx_buffer[con->tx_tail],
						   cnt))
	{
		con->tx_tail += cnt;

		if (con->tx_tail >= CONSOLE_TX_SIZE)
			con->tx_tail = 0;

		con->tx_busy = true;
	}
}

/*nalezy ponizsza funkcjedodac do callbackow transmisji stworzonych driverow*/

void Console_TxDone(console_t *con)
{
	if (con == NULL)
		return;

	con->tx_busy = false;

	/* jeżeli są kolejne dane, wyślij je od razu */
	Console_TxStart(con);
}

console_status_t Console_Init(console_t *con,
							  const console_driver_t *driver,
							  void *hw)
{

	if ((con == NULL) || (driver == NULL))
		return CONSOLE_ERROR;

	memset(con, 0, sizeof(console_t));

	con->driver = driver;
	con->hw = hw;

	return CONSOLE_OK;
}
bool Console_LineReady(const console_t *con)
{
    if (con == NULL)
        return false;

    return con->rx_line_ready;
}

const char *Console_GetLine(const console_t *con)
{
    if (con == NULL)
        return NULL;

    return con->line;
}

void Console_LineDone(console_t *con)
{
    if (con == NULL)
        return;

    con->rx_line_ready = false;
}

uint32_t Console_GetOverflow(const console_t *con)
{
    if (con == NULL)
        return 0;

    return con->tx_overflow;
}

bool Console_IsBusy(const console_t *con)
{
    if (con == NULL)
        return false;

    return con->tx_busy;
}
