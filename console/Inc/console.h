/*
 * console.h
 *
 *  Created on: Jul 31, 2026
 *      Author: jacabe
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>
#include <stdbool.h>

#define CONSOLE_RX_SIZE    64
#define CONSOLE_TX_SIZE    512

#ifndef CONSOLE_PRINTF_BUFFER_SIZE
#define CONSOLE_PRINTF_BUFFER_SIZE 128
#endif


typedef enum {
	CONSOLE_OK = 0,
	CONSOLE_ERROR,
	CONSOLE_BUSY,
	CONSOLE_BUFFER_FULL,
	CONSOLE_OVERFLOW
} console_status_t;

typedef struct
{
    bool (*Write)(void *hw,
                  const uint8_t *buf,
                  uint16_t len);
	bool (*Busy)(void *hw);
    void (*Flush)(void *hw);

} console_driver_t;


typedef struct {
	/*--------driver & hw--------*/
	const console_driver_t *driver;
	void *hw;

	/* ---------- RX ---------- */
	uint16_t rx_index;
	volatile bool rx_line_ready;
	char line[CONSOLE_RX_SIZE];

	/* ---------- TX ---------- */
	volatile bool tx_busy;
	char tx_buffer[CONSOLE_TX_SIZE];
	uint32_t tx_overflow;      // ilość utraconych znaków
	uint16_t tx_head;
	uint16_t tx_tail;

} console_t;



console_status_t Console_Init(console_t *con,
							  const console_driver_t *driver,
							  void *hw);


void Console_RxData(console_t *con,
					const uint8_t *buf,
					uint16_t len);

void Console_Printf(console_t *con, const char *fmt, ...);
console_status_t Console_Write(console_t *con,
							   const uint8_t *data,
							   uint16_t len);

void Console_TxStart(console_t *con);

void Console_TxDone(console_t *con);


bool Console_LineReady(const console_t *con);
const char *Console_GetLine(const console_t *con);
void Console_LineDone(console_t *con);
uint32_t Console_GetOverflow(const console_t *con);
bool Console_IsBusy(const console_t *con);

#endif /* CONSOLE_H_ */
