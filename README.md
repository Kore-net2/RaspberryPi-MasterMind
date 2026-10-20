# MasterMind on Raspberry Pi — C and ARM Assembly

**Status: university team coursework snapshot — source available, hardware and algorithm correctness NOT verified here.**  
Private archival copy of a MasterMind-style code-breaking game intended for Raspberry Pi 4 GPIO hardware. This repository is **not** a verified production-ready or fully functional project.

## Concept

The program aims to generate a hidden sequence of three colours, accept guesses through a push-button, drive red/green LEDs, and display feedback on an I²C 16×2 LCD. A separate ARM assembly routine (`mm-matches.s`) is intended to count exact and approximate sequence matches.

## Tech stack

- C (main game, GPIO and I²C functions)
- 32-bit ARM assembly and GCC/GNU assembler
- Raspberry Pi GPIO, LEDs, push-button and I²C LCD

## Files

| File | Purpose |
| --- | --- |
| `master-mind.c` | Main gameplay loop and hardware startup |
| `lcdBinary.c` | GPIO access and I²C LCD routines |
| `mm-matches.s` | ARM assembly sequence matcher (currently contains logic defects) |
| `Makefile` | Original coursework build commands |
| `test.sh`, `testm.c` | Original coursework test materials (not verified; signatures differ from current matcher) |
| `KNOWN_ISSUES.md` | Observed issues, compatibility and validation checklist |

## Hardware assumptions from source

| Component | Connection or assumption |
| --- | --- |
| Green LED | BCM GPIO 26 (with current-limiting resistor) |
| Red LED | BCM GPIO 5 (with current-limiting resistor) |
| Push-button | BCM GPIO 19 |
| I²C LCD module | `/dev/i2c-1`, address `0x27` (verify on actual device) |
| GPIO mapping | Hard-coded Raspberry Pi 4 peripheral base address `0xFE200000` |

**Caution:** The code uses direct `/dev/mem` access, privileged hardware registers, and 32-bit ARM inline assembly. Do not run it on a regular Windows PC or assume it works on 64-bit Raspberry Pi OS. Check the GPIO wiring and board compatibility before testing on a Pi. This version has not been repaired or bench-tested.

## Build (intended environment; unverified)

On an appropriately configured Raspberry Pi with compatible 32-bit compiler/OS and the right peripherals, the original Makefile specifies:

```sh
make master-mind
# DO NOT run with sudo until the logic and GPIO setup are reviewed.
```

See [KNOWN_ISSUES.md](KNOWN_ISSUES.md) first. `make all`, `make unit` and `make test` should not currently be treated as verified.

## Scope / authorship

The material was taken from a *team coursework* project archive for Heriot-Watt University's Hardware-Software Interface module. Some files began as course templates; the precise student contributions and redistribution permissions have not been independently established. Do not describe the entire codebase as individually authored or publish it without checking the course's academic-integrity and sharing requirements. No license is supplied.
