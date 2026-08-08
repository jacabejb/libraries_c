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
#include "communication_stream_driver.h"

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


typedef struct {
	/*--------driver & hw--------*/
	const communication_uart_driver_t *driver;
	void *hw;

	/* ---------- RX ---------- */
	uint16_t rx_index;
	volatile bool rx_line_ready;
	char line[CONSOLE_RX_SIZE];

	/* ---------- TX ---------- */
	volatile bool tx_busy;
	char tx_buffer[CONSOLE_TX_SIZE];
	char tx_printf_buffer[CONSOLE_PRINTF_BUFFER_SIZE];  /* printf formatting buffer */
	uint32_t tx_overflow;      // ilość utraconych znaków
	uint16_t tx_head;
	uint16_t tx_tail;

} console_t;



/******************************************************************************
 * @brief Initializes the console object.
 *
 * Initializes the console object, assigns the communication driver,
 * stores the hardware interface and prepares the internal buffers
 * for operation.
 *
 * This function only initializes the software object.
 * It does not transmit or receive any data.
 *
 * @param con
 * Pointer to the console object.
 *
 * @param driver
 * Pointer to the communication driver interface.
 *
 * @param hw
 * Driver-specific hardware handle.
 *
 * @return
 * Current console status.
 ******************************************************************************/
console_status_t Console_Init(console_t *obj,
                              const communication_uart_driver_t *driver,
                              void *hw);


							  

/******************************************************************************
 * @brief Passes received data to the console.
 *
 * Copies the received data into the internal receive buffer.
 * The function should be called whenever new data becomes available
 * from the communication driver.
 *
 * @param con
 * Pointer to the console object.
 *
 * @param buf
 * Pointer to the received data buffer.
 *
 * @param len
 * Number of received bytes.
 ******************************************************************************/
void Console_RxData(console_t *obj,
                    const uint8_t *buf,
                    uint16_t len);




/******************************************************************************
 * @brief Prints formatted text followed by a new line.
 *
 * Formats and transmits a text string and automatically appends
 * the "\r\n" sequence.
 *
 * @param obj
 * Pointer to console object.
 *
 * @param format
 * Printf-style format string.
 *
 * @param ...
 * Optional format arguments.
 *
 * @return
 * Operation status.
 ******************************************************************************/
console_status_t Console_PrintLine(console_t *obj, const char *format, ...);




/******************************************************************************
 * @brief Formats and transmits a text string.
 *
 * Formats the specified text using printf-style formatting and appends
 * the result to the console transmit buffer.
 *
 * Transmission is started automatically if the transmitter is idle.
 *
 * @param con
 * Pointer to the console object.
 *
 * @param fmt
 * Printf-style format string.
 ******************************************************************************/
console_status_t Console_Printf(console_t *obj,
                    const char *fmt,
                    ...);



/******************************************************************************
 * @brief Writes raw data to the transmit buffer.
 *
 * Copies the specified data into the console transmit buffer.
 *
 * Transmission is started automatically if the transmitter is idle.
 *
 * @param con
 * Pointer to the console object.
 *
 * @param data
 * Pointer to the data buffer.
 *
 * @param len
 * Number of bytes to write.
 *
 * @return
 * Current console status.
 ******************************************************************************/
console_status_t Console_Write(console_t *obj,
                               const uint8_t *data,
                               uint16_t len);



/******************************************************************************
 * @brief Starts transmission of buffered data.
 *
 * Starts a new transmission if the transmit buffer contains data and
 * no transmission is currently in progress.
 *
 * Normally this function is called internally by the console library.
 ******************************************************************************/
void Console_TxStart(console_t *obj);



/******************************************************************************
 * @brief Notifies the console that transmission has completed.
 *
 * This function should be called by the communication driver when the
 * current transmission has finished.
 *
 * If additional data are waiting in the transmit buffer, the next
 * transmission is started automatically.
 *
 * @param con
 * Pointer to the console object.
 ******************************************************************************/
void Console_TxDone(console_t *obj);



/******************************************************************************
 * @brief Checks whether a complete line has been received.
 *
 * @param con
 * Pointer to the console object.
 *
 * @return
 * true if a complete line is available, otherwise false.
 ******************************************************************************/
bool Console_LineReady(const console_t *obj);



/******************************************************************************
 * @brief Returns the last received line.
 *
 * Returns a pointer to the internal receive buffer containing the
 * most recently completed line.
 *
 * The returned pointer remains valid until Console_LineDone() is called.
 *
 * @param con
 * Pointer to the console object.
 *
 * @return
 * Pointer to the received null-terminated string.
 ******************************************************************************/
const char *Console_GetLine(const console_t *obj);



/******************************************************************************
 * @brief Releases the current receive line.
 *
 * Marks the current line as processed, allowing the console to receive
 * the next line.
 *
 * @param con
 * Pointer to the console object.
 ******************************************************************************/
void Console_LineDone(console_t *obj);



/******************************************************************************
 * @brief Returns the receive buffer overflow counter.
 *
 * @param con
 * Pointer to the console object.
 *
 * @return
 * Number of receive buffer overflows.
 ******************************************************************************/
uint32_t Console_GetOverflow(const console_t *obj);



/******************************************************************************
 * @brief Checks whether a transmission is in progress.
 *
 * @param con
 * Pointer to the console object.
 *
 * @return
 * true if the transmitter is busy, otherwise false.
 ******************************************************************************/
bool Console_IsBusy(const console_t *obj);

#endif /* CONSOLE_H_ */
