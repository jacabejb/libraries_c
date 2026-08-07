# Universal STM32 Libraries Framework

Version: 1.0.0

---

# English

## Purpose

This repository contains a collection of universal embedded libraries written in pure C.

The libraries are designed to be:

- portable
- hardware independent
- reusable
- multi-instance
- non-blocking
- easy to integrate into any STM32 project
- The library shall not know how hardware communication is implemented.

Communication method (polling, interrupt, DMA, RTOS, CMSIS, etc.) is selected by the driver implementation, not by the library.

Although STM32 is the primary target, the architecture allows easy porting to other platforms.

---

# General Rules

## 1. Pure C

Libraries shall be written in standard C (C11).

No C++ features.

---

## 2. No Dynamic Memory

The following functions shall never be used:

- malloc()
- calloc()
- realloc()
- free()

Application creates library objects.

Example:

```c
ads1115_t adc1;
console_t console;
```

---

## 3. Multi-instance

Every library object shall be completely independent.

Example:

```c
ads1115_t adc1;
ads1115_t adc2;
ads1115_t adc3;
```

Each object may use:

- different hardware
- different configuration
- different driver

---

## 4. Hardware Independence

Libraries shall never directly call HAL.

Communication is performed through driver interfaces.

Example:

```c
typedef struct
{
    xxx_status_t (*Write)(...);
    xxx_status_t (*Read)(...);

} xxx_driver_t;
```

---

## 5. Object-Oriented Design

All library state shall be stored inside the object.

No global variables.

---

## 6. Status Handling

Every object contains:

```c
status
```

Every function returning

```c
xxx_status_t
```

must:

1. update

```c
object->status
```

2. return exactly the same value.

Example:

```c
object->status = XXX_OK;
return object->status;
```

Never:

```c
return XXX_OK;
```

---

## 7. Data Integrity

Functions shall never overwrite valid data with invalid data.

Example:

```
Read() == OK
```

↓

update object data

```
Read() == BUSY
Read() == ERROR
Read() == TIMEOUT
```

↓

keep previous valid data.

---

## 8. Single Responsibility

Every function shall perform only one task.

Typical API:

```
Init()

Set...

Request...

Read...

Get...
```

---

## 9. Getters

Getter functions never perform calculations.

They only return data stored inside the object.

All calculations are performed during Read().

---

## 10. Non-blocking

Libraries shall never wait using:

```
HAL_Delay()

while(...)
```

Operation completion is checked by calling Read() periodically.

---

## 11. Configuration

Library configuration shall be stored inside the object.

Example:

```c
uint16_t config;
```

Setter functions modify configuration.

Request() builds the complete device register and sends it to hardware.

---

## 12. Integer Arithmetic

Whenever possible libraries should use integer arithmetic.

Preferred units:

- mV
- mA
- m°C
- ms

Floating point should only be used when absolutely necessary.

---

## 13. API Stability

Once a public function is released it should never change.

New functionality shall be added using new functions.

Never break existing applications.

---

## 14. Naming Convention

Public API:

```
XXX_Init()

XXX_Set...

XXX_Request...

XXX_Read...

XXX_Get...
```

---

## 15. Source Code Language

All source code shall use English only.

Including:

- comments
- variable names
- structure names
- function names
- debug messages

---

## 16. Documentation

Every library shall contain:

```
README.md

CHANGELOG.md

LICENSE
```

Documentation shall always be bilingual.

Order:

1. English
2. Polish

---

## 17. Folder Structure

```
library_name/

├── Inc/
├── Src/
├── README.md
├── CHANGELOG.md
├── LICENSE
└── examples/
```

---

# Polski

## Cel

Repozytorium zawiera zbiór uniwersalnych bibliotek dla systemów embedded napisanych w języku C.

Biblioteki projektowane są jako:

- przenośne,
- niezależne od sprzętu,
- wieloinstancyjne,
- nieblokujące,
- łatwe do wykorzystania w dowolnym projekcie STM32.
- Biblioteka nie może wiedzieć, w jaki sposób realizowana jest komunikacja ze sprzętem.

---

## Ogólne zasady

### 1. Czysty język C

Biblioteki pisane są w standardzie C11.

Nie używamy C++.

---

### 2. Brak dynamicznej pamięci

Nigdy nie używamy:

- malloc()
- calloc()
- realloc()
- free()

Obiekty tworzy aplikacja.

---

### 3. Wieloinstancyjność

Każdy obiekt działa całkowicie niezależnie.

---

### 4. Niezależność od HAL

Biblioteka nigdy nie wywołuje bezpośrednio funkcji HAL.

Komunikacja odbywa się przez driver.

---

### 5. Brak zmiennych globalnych

Cały stan biblioteki znajduje się w strukturze obiektu.

---

### 6. Obsługa statusu

Każda funkcja zwracająca status:

- zapisuje go do `object->status`,
- zwraca dokładnie tę samą wartość.

---

### 7. Integralność danych

Nie wolno nadpisywać poprawnych danych błędnymi.

Jeżeli odczyt zakończy się błędem, obiekt nadal zawiera ostatni poprawny pomiar.

---

### 8. Jedna odpowiedzialność

Każda funkcja wykonuje tylko jedno zadanie.

```
Init()
Set...
Request...
Read...
Get...
```

---

### 9. Gettery

Funkcje Get() jedynie zwracają dane.

Nigdy niczego nie obliczają.

---

### 10. Praca nieblokująca

Nie używamy:

- HAL_Delay()
- pętli oczekujących

Stan operacji sprawdzany jest przez Read().

---

### 11. Konfiguracja

Konfiguracja układu przechowywana jest w strukturze obiektu.

Settery modyfikują konfigurację.

Request() buduje pełny rejestr urządzenia.

---

### 12. Liczby całkowite

Jeżeli to możliwe używamy jednostek całkowitych:

- mV
- mA
- m°C
- ms

Float tylko wtedy, gdy jest to konieczne.

---

### 13. Stabilność API

Po opublikowaniu funkcji publicznej nie zmieniamy jej interfejsu.

Nowe możliwości dodajemy przez nowe funkcje.

---

### 14. Nazewnictwo

Wszystkie biblioteki stosują identyczny schemat:

```
Init()

Set...

Request...

Read...

Get...
```

---

### 15. Język kodu

Kod źródłowy zawiera wyłącznie język angielski.

Dotyczy:

- komentarzy,
- nazw zmiennych,
- nazw funkcji,
- nazw struktur,
- komunikatów debug.

---

### 16. Dokumentacja

Każda biblioteka zawiera:

```
README.md
CHANGELOG.md
LICENSE
```

Dokumentacja jest zawsze dwujęzyczna.

Kolejność:

1. English
2. Polski

---

### 17. Struktura katalogów

```
library_name/

├── Inc/
├── Src/
├── README.md
├── CHANGELOG.md
├── LICENSE
└── examples/
```
