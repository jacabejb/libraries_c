# Common

Shared communication interfaces used by the STM32 Universal Libraries framework.

The files in this directory define generic communication interfaces
between libraries and hardware drivers.

This abstraction allows every library to remain hardware independent.

---

# Available Interfaces

- communication_i2c_driver.h
- communication_stream_driver.h

---

# Purpose

Libraries never communicate directly with STM32 HAL or any other
hardware-specific API.

Instead, they use a generic communication interface provided by
the application.

This allows the same library to work with:

- STM32 HAL
- LL Drivers
- Zephyr
- FreeRTOS projects
- Custom drivers

without any source code modifications.

---

# Driver Implementations

Example driver implementations included with the framework:

- I2C Blocking
- I2C Interrupt
- UART DMA
- UART Interrupt
- USB CDC

Users may also provide their own driver implementation by implementing
the corresponding communication interface.

+----------------------------+
|        Application         |
+-------------+--------------+
              |
              |
+-------------v--------------+
| communication_*_driver_t   |
+-------------+--------------+
              |
      +-------+-------+
      |               |
+-----v-----+   +-----v------+
| STM32 HAL |   | User Driver|
+-----------+   +------------+

---

# License

MIT License