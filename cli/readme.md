# CLI

Universal command-line interface (CLI) library for embedded applications.

The library is part of the STM32 Universal Libraries framework and
provides a lightweight, hardware-independent command parser that
integrates with the Console library.

---

# Features

- Pure C (C11)
- Hardware independent
- No dynamic memory allocation
- Multi-instance support
- Automatic command lookup
- Automatic help generation
- Integer argument parser
- Floating-point argument parser (stored as integer)
- String argument parser
- User callback mechanism
- Command diagnostics
- Configurable commentary messages
- Designed for embedded systems

---

# Supported argument types

- No argument
- Integer
- Floating-point
- String

Floating-point values are automatically converted to integers before
being passed to the callback function.

# Architecture

The library separates:

- command parsing,
- argument conversion,
- callback execution,
- diagnostics.

The application only defines a command table and implements the
associated callback functions.

Example:

```
weight:1.86
```

becomes

```
186
```

allowing applications to avoid floating-point calculations.

---

# Requirements

- Console library

---

# Example

```c
static const cli_cmd_table_t table[] =
{
    { "save",     CLI_ARG_NONE,   SaveCallback,     "Save parameters" },
    { "load",     CLI_ARG_NONE,   LoadCallback,     "Load parameters" },
    { "weight:",  CLI_ARG_FLOAT,  WeightCallback,   "Calibration weight" },
    { "name:",    CLI_ARG_STRING, NameCallback,     "Device name" }
};

CLI_Init(&cli,
         &console,
         table,
         CLI_TABLE_SIZE(table));

while (1)
{
    CLI_Update(&cli);
}
```

---

# Built-in commands

The library provides one built-in command:

```
help
```

which automatically generates a list of all registered commands and
their descriptions.

---

# API

The complete API documentation is available in:

```
Inc/cli.h
```

---

# License

MIT License