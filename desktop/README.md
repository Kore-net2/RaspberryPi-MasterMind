# MasterMind Desktop Browser Simulator

**Open `index.html` in Edge or Chrome on Windows.** It also works offline; no Raspberry Pi, C compiler, .NET, Node.js, web server or downloads needed.

This is a **new, independent browser reimplementation** of the MasterMind game rules from the university Raspberry Pi project. It doesn't execute or repair the original ARM assembly/GPIO implementation.

- Secret has 3 colour values: `1` green, `2` red, `3` blue.
- Colour values **may repeat** (for example, `112`).
- `exact`: right colour and position. `approx`: right colour, different position, counting each colour occurrence once.
- Choose three coloured pegs and press **Check combination**, or use number keys `1`, `2`, `3` and Enter.
- Unlimited guesses, history, new game, reveal, and optional fixed-code demo mode.
- For a repeatable demo, expand **Demo / reproducible test mode**, type `123`, press **New game**, submit `132` (1 exact, 2 approx) and `123` (win).

## Automated tests

Open **`tests.html`** in the browser: checks all 729 possible secret/guess combinations against an independent matcher, plus validation and game-state tests. The same test file can run with Node.js if installed: `node tests.js`.

Note: This interactive demo is intended for PC teaching and interviews. The original Raspberry Pi source is kept at repository root and still needs hardware-specific debugging.
