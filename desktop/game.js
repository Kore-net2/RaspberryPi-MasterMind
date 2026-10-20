/* Standalone browser/Node implementation of the original game's scoring rules.
   This is a NEW Windows-friendly simulation, not a Raspberry Pi hardware driver. */
(function (root, factory) {
  const api = factory();
  if (typeof module === "object" && module.exports) module.exports = api;
  root.MasterMind = api;
})(typeof globalThis !== "undefined" ? globalThis : this, function () {
  "use strict";

  const LENGTH = 3;
  const COLORS = 3;

  function normalizeCode(raw) {
    let code;
    if (typeof raw === "string") {
      code = raw.trim().split("").map(c => Number(c));
    } else if (Array.isArray(raw)) {
      code = raw.slice();
    } else {
      throw new TypeError("A code must be an array or a 3-digit string.");
    }
    if (code.length !== LENGTH || !code.every(n => Number.isInteger(n) && n >= 1 && n <= COLORS)) {
      throw new RangeError("Enter exactly three colours, using only 1, 2 and 3.");
    }
    return code;
  }

  function scoreGuess(secretInput, guessInput) {
    const secret = normalizeCode(secretInput);
    const guess = normalizeCode(guessInput);
    const unmatchedSecret = Array(COLORS + 1).fill(0);
    const unmatchedGuess = Array(COLORS + 1).fill(0);
    let exact = 0;
    let approx = 0;
    for (let i = 0; i < LENGTH; i++) {
      if (secret[i] === guess[i]) exact++;
      else {
        unmatchedSecret[secret[i]]++;
        unmatchedGuess[guess[i]]++;
      }
    }
    for (let color = 1; color <= COLORS; color++) {
      approx += Math.min(unmatchedSecret[color], unmatchedGuess[color]);
    }
    return { exact, approx };
  }

  function randomCode() {
    return Array.from({ length: LENGTH }, () => Math.floor(Math.random() * COLORS) + 1);
  }

  function createGame(secretInput) {
    const secret = secretInput === undefined ? randomCode() : normalizeCode(secretInput);
    const history = [];
    let status = "playing";
    return {
      get status() { return status; },
      get history() { return history.map(item => ({ guess: item.guess.slice(), exact: item.exact, approx: item.approx })); },
      get attempts() { return history.length; },
      submit(guessInput) {
        if (status !== "playing") throw new Error("This game has ended. Start a new game.");
        const guess = normalizeCode(guessInput);
        const result = scoreGuess(secret, guess);
        const entry = { guess: guess.slice(), ...result };
        history.push(entry);
        if (result.exact === LENGTH) status = "won";
        return { ...entry, won: status === "won", attempts: history.length };
      },
      reveal() {
        if (status === "playing") status = "revealed";
        return secret.slice();
      }
    };
  }

  return Object.freeze({ LENGTH, COLORS, normalizeCode, scoreGuess, createGame });
});
