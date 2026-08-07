# ADS1115

Portable, non-blocking driver for the Texas Instruments ADS1115
16-bit I²C analog-to-digital converter.

The library is part of the STM32 Universal Libraries framework and
provides a unified API compatible with other ADC libraries.

---

## Features

- Pure C (C11)
- Multi-instance
- Hardware independent
- Non-blocking state machine
- Blocking and interrupt I²C support
- Single channel measurements
- Sequential channel scanning
- One-shot mode
- Continuous mode
- Programmable gain amplifier (PGA)
- Configurable conversion rate

---

## Requirements

- communication_i2c_driver_t
- STM32 HAL driver examples included

---

## Example

See:

```
examples/STM32CubeIDE/
```

---

## API

The complete API documentation is available in:

```
Inc/ads1115.h
```

---

## License

MIT License