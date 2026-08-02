# CLI Library

## Opis

Biblioteka realizuje parser tekstowych komend.

Nie zależy od transportu.

Może współpracować z:

- Console USB
- Console UART
- dowolnym źródłem tekstu

---

## Przepływ

Console

↓

CLI

↓

Application

---

## Funkcje

### CLI_Parse()

```c
CLI_Parse(console_t *console,
          cli_result_t *result);
```

Sprawdza, czy dostępna jest nowa linia.

Jeżeli tak:

- pobiera tekst
- rozpoznaje komendę
- odczytuje argument
- zwalnia bufor

---

## Obsługiwane argumenty

- brak argumentu

```
save
```

- liczba całkowita

```
engine:21
```

```
step:-150
```

---

## Wynik

```c
result.ready
```

Odebrano komendę.

```c
result.valid
```

Argument poprawny.

```c
result.cmd
```

Kod komendy.

```c
result.value
```

Argument liczbowy.