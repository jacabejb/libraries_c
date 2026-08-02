# Console Library

## Opis

Biblioteka realizuje uniwersalną konsolę tekstową niezależną od sprzętu.

Obsługiwane interfejsy realizowane są przez sterowniki (drivers):

- USB CDC
- UART DMA
- UART Interrupt
- UART Polling

Jedna aplikacja może posiadać wiele niezależnych konsol.

Przykład:

```c
console_t console_usb;
console_t console_uart2;
```

---

## Funkcje

### Console_Init()

```c
console_status_t Console_Init(console_t *con,
                              const console_driver_t *driver,
                              void *hw);
```

Inicjalizacja konsoli.

Parametry:

- con – instancja konsoli
- driver – sterownik transportu
- hw – uchwyt sprzętowy (UART_HandleTypeDef, USB itd.)

---

### Console_Printf()

```c
Console_Printf(console_t *con,
               const char *fmt,
               ...);
```

Dodaje sformatowany tekst do kolejki TX.

---

### Console_Write()

```c
Console_Write(console_t *con,
              const uint8_t *data,
              uint16_t len);
```

Dodaje dane binarne do kolejki TX.

---

### Console_RxData()

```c
Console_RxData(console_t *con,
               const uint8_t *buf,
               uint16_t len);
```

Przekazuje odebrane dane do parsera linii.

Wywoływana z callbacka sterownika.

---

### Console_TxDone()

Wywoływana z callbacka zakończenia transmisji.

Rozpoczyna wysyłanie kolejnego fragmentu FIFO.

---

### Console_LineReady()

Sprawdza czy odebrano pełną linię zakończoną ENTER.

---

### Console_GetLine()

Zwraca wskaźnik na odebraną linię.

---

### Console_LineDone()

Czyści flagę odebranej linii.

---

## Diagram

Application

↓

Console

↓

Driver (USB/UART)

↓

Hardware