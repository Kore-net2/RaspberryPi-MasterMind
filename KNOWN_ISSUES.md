# Known issues and validation checklist

This document records problems visible during static inspection. No Raspberry Pi hardware execution or successful compilation was performed in the packaging environment. Original program sources were **not edited**.

## Observed technical issues

1. `mm-matches.s` contains `CMP R8, R8`, which always evaluates equal. The approximate-match branch therefore cannot compare two different values correctly. There are also indexing issues when accessing `int[]` values with byte loads (`LDRB`) and potentially overlapping scratch arrays on the stack. **Matching accuracy should not be claimed.**
2. In `lcdBinary.c`, inline assembly in `pinMode` appears not to use the `pin` argument properly and uses registers inconsistent with its operand list. GPIO direction setup should be audited before real hardware use.
3. `master-mind.c` opens `/dev/mem` and maps a hard-coded Raspberry Pi 4 GPIO address before doing anything with command-line unit testing. Therefore `-u` is not a hardware-free test mode. It is unsuitable as-is for desktop execution.
4. The original `testm.c` uses an older/different `matches` function signature, and the original `Makefile` builds it by default. `make all` and `make test` are **not proven compatible** with the current source.
5. The demo text `Smith` in `master-mind.c` is a remaining coursework placeholder, not evidence of project authorship.
6. The original main program prints the secret sequence when compiled with `#define DEBUG`; disable this for a real guessing game.
7. I²C address and physical wiring are hard-coded; match them to the actual Pi and module. Timing uses a busy-wait `delay()` whose duration depends on CPU/compiler.

## Recommended validation before a public showcase

- Confirm which code sections were written by your team and which are instructor starter templates.
- Verify redistribution rights and academic-integrity restrictions before setting the repo to Public.
- Audit/fix the assembly matcher, GPIO register operations, and argument/stack conventions.
- Compile with the compatible Raspberry Pi toolchain and address compiler diagnostics.
- Replace the nonportable busy-wait with proper timing, if permitted by assignment rules.
- Write independent deterministic tests for exact and approximate matching, including repeated colours.
- Run hardware tests with known-good wiring and a safe GPIO setup, and record actual results.
- Add photos or demonstration video you own, only with consent of any teammates appearing in the media.

Until then, refer to this as a **team coursework prototype / archival code**, not a completed verified game.

## PC additions (independently implemented and tested)

The new `desktop/` browser version and the `portable-c/` reference implementation apply correct exact/approximate scoring with duplicate colours. Both matching implementations passed exhaustive 27 × 27 = 729 combination tests against independent reference matchers on the preparation machine. The C program and its test executable compiled with GCC on Linux; the browser JavaScript matcher was tested with Node.js. The visible HTML page was not end-to-end tested here due to browser policy restrictions. Windows execution of the optional native C version requires a C compiler.

**Remaining open issues are all for the original Raspberry Pi implementation:** the original `mm-matches.s`, GPIO access and original `Makefile` were not changed or validated. Do not claim that the ARM assembly or physical game was fixed.
