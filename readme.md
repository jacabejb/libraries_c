# STM32 Universal Libraries

Portable embedded libraries with a unified API for STM32 and other embedded platforms.

A collection of portable, reusable and hardware-independent libraries written in pure C (C11). The project follows a unified framework, providing a consistent API, architecture and coding style across all libraries.

---

# Available Libraries

- Console
- ADS1115
- MCP3424

---

# Communication Drivers

- I2C Blocking
- I2C Interrupt
- UART DMA
- UART Interrupt
- USB

---

# Planned Libraries

- VT100 Terminal
- Settings
- Cli
...

---

# Features

- Pure C (C11)
- Multi-instance
- Hardware independent
- Non-blocking state machines
- Communication abstraction
- No dynamic memory allocation
- Portable architecture
- Unified API
- Easy integration with STM32CubeIDE

---

# Requirements

The libraries are hardware independent.

Example communication drivers are provided for STM32 HAL.

---

# Installation

Each library is fully independent and may be added separately.

Add the required `Inc` and `Src` directories to your project as:

- Linked Folder (recommended)
- Source Folder

Alternatively, copy the selected library directly into your project.

---

# Documentation

Each library contains:

- API documentation
- Usage examples
- Driver interface description
- Changelog

See `framework.md` for the complete framework specification and design rules.

---

# License

MIT License

---

# Biblioteki Uniwersalne STM32

Zbiór przenośnych, wielokrotnego użytku bibliotek napisanych w czystym języku C (C11) dla mikrokontrolerów STM32 oraz innych platform embedded.

Projekt opiera się na jednolitym frameworku zapewniającym spójne API, architekturę oraz styl implementacji wszystkich bibliotek.

---

# Dostępne biblioteki

- Console
- CLI
- ADS1115
- MCP3424

---

# Sterowniki komunikacyjne

- I2C Blocking
- I2C Interrupt
- UART DMA
- UART Interrupt
- USB

---

# Biblioteki planowane

- Terminal VT100
- Settings
- ADS1256

---

# Cechy

- Czysty język C (C11)
- Wieloinstancyjność
- Niezależność od sprzętu
- Nieblokujące maszyny stanów
- Warstwa abstrakcji komunikacji
- Brak dynamicznej alokacji pamięci
- Przenośna architektura
- Ujednolicone API
- Łatwa integracja z STM32CubeIDE

---

# Wymagania

Biblioteki są niezależne od warstwy sprzętowej.

Przykładowe sterowniki komunikacyjne zostały przygotowane dla STM32 HAL.

---

# Instalacja

Każda biblioteka jest całkowicie niezależna i może zostać dodana do projektu osobno.

Dodaj katalogi `Inc` oraz `Src` wybranej biblioteki do projektu jako:

- Linked Folder (zalecane)
- Source Folder

Można również skopiować wybraną bibliotekę bezpośrednio do projektu.

---

# Dokumentacja

Każda biblioteka zawiera:

- dokumentację API,
- przykłady użycia,
- opis interfejsu sterownika,
- historię zmian.

Szczegółowy opis architektury frameworka znajduje się w pliku `framework.md`.

---

# Licencja

MIT License