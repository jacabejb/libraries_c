# Button Library

Version: 1.0.0

---

# English

## Overview

The Button library provides a universal, multi-instance button handler written in pure C.

The library supports:

- configurable software debounce
- short press detection
- long press detection
- short release detection
- long release detection
- auto-repeat
- polling mode
- external interrupt (EXTI) mode

The library is completely hardware independent and operates on the universal GPIO driver.

---

## Features

- Pure C
- Multi-instance
- Hardware independent
- Platform independent
- Polling mode
- EXTI mode
- Stable software debounce
- Configurable debounce time
- Configurable short press time
- Configurable long press time
- Configurable repeat delay
- Configurable repeat interval
- Runtime callback registration
- User data support
- No dynamic memory allocation
- No blocking delays

---

## Directory structure

```
button/
│
├── Inc/
│   └── button.h
│
├── Src/
│   └── button.c
│
├── README.md
└── LICENSE
```

---

## Dependencies

The library requires:

- driver_gpio

The GPIO driver is responsible for maintaining the current pin state.

The Button library operates only on the GPIO driver state and is completely
independent of the underlying hardware access method.

Both Polling and EXTI modes use exactly the same Button library code.

---

## Button events

The library generates the following events:

| Event | Description |
|---------|-------------|
| BUTTON_IDLE | No button activity |
| BUTTON_PRESSED | Button press detected after debounce |
| BUTTON_HELD_SHORT | Short press time elapsed |
| BUTTON_HELD_LONG | Long press time elapsed |
| BUTTON_RELEASED_SHORT | Button released after short press |
| BUTTON_RELEASED_LONG | Button released after long press |
| BUTTON_REPEAT | Auto-repeat event |

---

## Polling mode

In Polling mode the GPIO driver reads the hardware state every time
`Button_Update()` is called.

```
Application
      │
      ▼
Button_Update()
      │
      ▼
Driver_GPIO_ReadPin()
      │
      ▼
HAL_GPIO_ReadPin()
```

The debounce algorithm validates every detected state change before generating
button events.

---

## EXTI mode

In EXTI mode the GPIO driver updates the GPIO state inside the interrupt
callback.

```
GPIO Interrupt
      │
      ▼
Driver_GPIO_EXTI_Callback()
      │
      ▼
driver_gpio.state updated
```

The application shall continue calling `Button_Update()` periodically.

All debounce processing, timing and callback execution are performed outside
the interrupt.

Therefore Polling and EXTI modes use exactly the same application code.

---

## Callback mechanism

Callbacks are optional.

If a callback is not registered, the corresponding event is ignored.

Each button instance owns its own callback table.

Example:

```c
static const button_callbacks_t callbacks =
{
    .short_pressed  = Btn1ShortPress,
    .short_released = Btn1ShortRelease,
    .long_pressed   = Btn1LongPress,
    .long_released  = Btn1LongRelease,
    .repeated       = Btn1Repeat
};
```

Callback prototype:

```c
void Btn1ShortPress(void *obj)
{
    Console_PrintLine(&console_usb,
                      "*** Button 1 Short Press ***");
}
```

The callback receives the pointer passed as `user_data` during
`Button_Init()`.

Example:

```c
Button_Init(
    &button1,
    &driver_btn1,
    ACTIVE_HIGH,
    REPEAT,
    50,
    350,
    1200,
    1000,
    300,
    &callbacks,
    &button1);
```

Inside the callback:

```c
void Btn1ShortPress(void *obj)
{
    button_t *button = (button_t *)obj;

    Console_PrintLine(&console_usb,
                      "*** Button 1 Short Press ***");
}
```

---

### Callback events

| Callback | Description |
|-----------|-------------|
| short_pressed | Called once after the configured short press time. |
| short_released | Called when the button is released after a short press. |
| long_pressed | Called once after the configured long press time. |
| long_released | Called when the button is released after a long press. |
| repeated | Called periodically after the repeat delay while the button remains pressed. |

---

## Initialization

```c
driver_gpio_t driver_btn1;

button_t btn1;

Driver_GPIO_Init(
    &driver_btn1,
    sw_GPIO_Port,
    sw_Pin,
    DRIVER_GPIO_MODE_POLLING);

Button_Init(
    &btn1,
    &driver_btn1,
    ACTIVE_HIGH,
    REPEAT,
    50,
    350,
    1200,
    1000,
    300,
    &callbacks,
    &btn1);
```

All timing parameters are expressed in milliseconds.

| Parameter | Description |
|-----------|-------------|
| debounce | Debounce filter time |
| short press | Time before `short_pressed` event |
| long press | Time before `long_pressed` event |
| repeat delay | Delay before first repeat event |
| repeat interval | Time between repeat events |

---

## Periodic update

The application shall periodically call:

```c
Button_Update(&btn1, HAL_GetTick());
```

`HAL_GetTick()` (or any equivalent timer) shall return the current system
time in milliseconds.

The update function may be called from the main loop or from a periodic task.

---

## Debounce algorithm

The library implements a software debounce algorithm.

A GPIO state change is accepted only if the new state remains stable for the
configured debounce time.

Only after the debounce period expires are button events generated.

This approach eliminates false events caused by mechanical contact bouncing
and guarantees identical behaviour in both Polling and EXTI modes.

---

## Runtime configuration

The following parameters may be modified during runtime:

- debounce time
- short press time
- long press time
- repeat enable
- repeat delay
- repeat interval
- callback table

New configuration values become effective immediately.

---

## Event sequence

Typical event order:

```
Button pressed
        │
        ▼
BUTTON_PRESSED
        │
        ▼
BUTTON_HELD_SHORT
        │
        ▼
BUTTON_HELD_LONG
        │
        ▼
BUTTON_REPEAT
        │
        ▼
BUTTON_REPEAT
        │
       ...
        │
Button released
        │
        ▼
BUTTON_RELEASED_LONG
```

If the button is released after the short press event but before the long
press event, the generated sequence becomes:

```
BUTTON_PRESSED
        │
        ▼
BUTTON_HELD_SHORT
        │
        ▼
BUTTON_RELEASED_SHORT
```

If the button is released before the configured short press time, only:

```
BUTTON_PRESSED
        │
        ▼
BUTTON_IDLE
```

is generated.

---

## Notes

- Supports unlimited independent button instances.
- Polling and EXTI use exactly the same application code.
- Fully hardware independent.
- No dynamic memory allocation.
- No blocking delays.
- Suitable for bare-metal applications.
- Suitable for RTOS applications.

---

## License

MIT License.