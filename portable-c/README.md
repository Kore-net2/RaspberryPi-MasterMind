# Portable C MasterMind (Windows/Linux/macOS)

A **new, hardware-independent reference implementation** of the original coursework game's rules: three slots, three colours (1=green, 2=red, 3=blue), duplicates allowed. The original Raspberry Pi/ARM assembly files at the repository root are **unchanged**; this does **not** fix or validate the Raspberry Pi GPIO code.

## Windows with GCC or MSVC

From PowerShell in this folder:

```powershell
.\build.ps1
.\mastermind.exe --secret 123
```

Enter `123` to win; `132` produces **1 exact / 2 approximate**. Quit with `q`. Omit `--secret 123` for a random secret.

You need `gcc` on PATH (such as MSYS2/MinGW-w64) or `cl` in a Visual Studio Developer PowerShell. If neither is installed, **the browser version in `../desktop/index.html` runs without any compiler**.

## Linux/macOS

```sh
cc -std=c11 -Wall -Wextra -Werror -pedantic matcher.c test_matcher.c -o test_matcher
./test_matcher
cc -std=c11 -Wall -Wextra -Werror -pedantic matcher.c main.c -o mastermind
./mastermind --secret 123
```

## Matching algorithm

First remove exact matches. For all unmatched positions, count each colour in the secret and guess; for every colour, add the **smaller** of those counts to approximate matches. This prevents using the same colour occurrence more than once.

`test_matcher.c` checks **all 729 possible secret/guess combinations** against an independent reference matcher, plus repeated colours and invalid input. No external C dependencies.
