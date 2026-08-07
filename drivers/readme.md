# Drivers

Hardware-specific communication drivers for the STM32 Universal Libraries framework.

The drivers in this directory implement the generic communication
interfaces defined in the `common` module.

Libraries included in the framework never communicate directly with
STM32 HAL. Instead, they use one of these communication drivers.

---

# Available Drivers

## I2C

- Blocking
- Interrupt

Implements:

```
communication_i2c_driver_t
```

---

## UART

- DMA
- Interrupt

Implements:

```
communication_uart_driver_t
```

---

## USB

- USB CDC

Implements:

```
communication_uart_driver_t
```

---

# Architecture

```
Application
      │
      ▼
+------------------------+
|   Device Library       |
|  ADS1115 / MCP3424     |
|  Console / CLI         |
+-----------+------------+
            │
            ▼
+------------------------+
| communication_*_driver |
+-----------+------------+
            │
            ▼
+------------------------+
| STM32 HAL Driver       |
+------------------------+
```

---

# Adding Custom Drivers

Any communication backend may be used by implementing the corresponding
communication interface defined in the `common` module.

This allows the framework libraries to be used with:

- STM32 HAL
- STM32 LL
- Zephyr
- FreeRTOS
- Bare-metal projects
- Custom communication layers

without modifying the library source code.

---

# License

MIT License