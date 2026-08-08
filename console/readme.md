# Console

Portable console library for embedded applications.

The library is part of the STM32 Universal Libraries framework and
provides a hardware-independent console interface for sending and
receiving text over any stream-based communication driver.

---

# Features

- Pure C (C11)
- Multi-instance
- Hardware independent
- Circular transmit buffer
- Circular receive buffer
- Non-blocking transmission
- Line-oriented reception
- printf()-style formatted output
- Raw binary transmission
- Communication diagnostics
- Compatible with CLI library

---

# Requirements

- communication_uart_driver_t
- STM32 HAL driver examples included

---

# Example

See:

```
examples/STM32CubeIDE/
```

---

# API

The complete API documentation is available in:

```
Inc/console.h
```

---

# License

MIT License