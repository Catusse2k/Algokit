
# Algokit

A small C++17 toolkit of classic algorithms with built-in self-tests and no external dependencies.

## Features

- **QuantumHeap**: generic heap (max-heap by default, min-heap with `std::greater`)
- **Run-Length Encoding**: encoding and decoding with input validation
- **Sieve of Eratosthenes**: prime number generation up to a given limit
- **Convex hull**: monotone chain algorithm with polygon area calculation

## Build

Requires a compiler with C++17 support.

Linux / macOS:

```
g++ -std=c++17 -O2 -Wall -Wextra main_catusse.cc -o catusse
```

Windows (MinGW):

```
g++ -std=c++17 -O2 -Wall -Wextra -static main_catusse.cc -o catusse.exe
```

## Usage

```
./catusse [--seed N] [--no-selftest]
```

| Option | Description |
|---|---|
| `--seed N` | Use seed `N` to make the output reproducible |
| `--no-selftest` | Skip the initial self-tests |

Without options the seed is random on every run and the self-tests run before anything else.

## Example output

```
[INFO] avvio (seed=42)
[INFO] --- self-test ---
[TEST] OK    heap max ordine
...
[INFO] --- fine self-test: tutto ok ---
[INFO] RLE di "aaabbbccddddeeee": 3a3b2c4d4e -> decodifica: aaabbbccddddeeee
[INFO] Primi fino a 100: 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97
```

Program messages are in Italian.

## Exit codes

| Code | Meaning |
|---|---|
| `0` | Completed successfully |
| `1` | Self-tests failed or runtime error |
| `2` | Unrecognized command-line argument |

## Notes

- The RLE encoder rejects digits in the input, because they would make the `<count><character>` format ambiguous.
- On Windows, avoid folder names containing `;`: `cmd` treats it as a command separator.
