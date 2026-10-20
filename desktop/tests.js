/* Run with: node tests.js. Browser: open tests.html. No dependencies. */
(function (root) {
  "use strict";
  const MM = typeof module === "object" && module.exports ? require("./game.js") : root.MasterMind;
  let assertions = 0;

  function equal(actual, expected, message) {
    assertions++;
    if (actual !== expected) throw new Error(`${message}: expected ${expected}, received ${actual}`);
  }
  function throws(fn, message) {
    assertions++;
    let thrown = false;
    try { fn(); } catch (_) { thrown = true; }
    if (!thrown) throw new Error(`${message}: expected an exception`);
  }
  function referenceScore(secret, guess) {
    const usedS = [false, false, false];
    const usedG = [false, false, false];
    let exact = 0, approx = 0;
    for (let i = 0; i < 3; i++) {
      if (secret[i] === guess[i]) { usedS[i] = true; usedG[i] = true; exact++; }
    }
    for (let i = 0; i < 3; i++) {
      if (usedG[i]) continue;
      for (let j = 0; j < 3; j++) {
        if (!usedS[j] && guess[i] === secret[j]) {
          usedS[j] = true;
          approx++;
          break;
        }
      }
    }
    return { exact, approx };
  }
  function allCodes() {
    const codes = [];
    for (let a = 1; a <= 3; a++)
      for (let b = 1; b <= 3; b++)
        for (let c = 1; c <= 3; c++) codes.push([a, b, c]);
    return codes;
  }
  function runTests() {
    const codes = allCodes();
    let pairs = 0;
    for (const s of codes) {
      for (const g of codes) {
        const got = MM.scoreGuess(s, g);
        const expected = referenceScore(s, g);
        equal(got.exact, expected.exact, `${s} / ${g} exact`);
        equal(got.approx, expected.approx, `${s} / ${g} approx`);
        equal(got.exact + got.approx <= 3, true, `${s} / ${g} max`);
        pairs++;
      }
    }
    equal(MM.scoreGuess("123", "132").exact, 1, "one exact");
    equal(MM.scoreGuess("123", "132").approx, 2, "two approximate");
    equal(MM.scoreGuess("112", "221").approx, 2, "duplicates not double-counted");
    equal(MM.scoreGuess("111", "222").approx, 0, "no matches");
    throws(() => MM.scoreGuess("123", "042"), "invalid zero");
    throws(() => MM.scoreGuess("123", "1234"), "invalid length");
    throws(() => MM.scoreGuess("123", [1, 2, 2.5]), "invalid fractional value");
    const game = MM.createGame("123");
    equal(game.attempts, 0, "initial attempts");
    const first = game.submit("132");
    equal(first.won, false, "first attempt not a win");
    equal(game.status, "playing", "game still playing");
    const second = game.submit("123");
    equal(second.won, true, "winning guess");
    equal(game.attempts, 2, "two attempts");
    equal(game.status, "won", "game won");
    throws(() => game.submit("123"), "cannot guess after game won");
    const revealed = MM.createGame("231");
    equal(revealed.reveal().join(""), "231", "reveal secret");
    equal(revealed.status, "revealed", "revealed game ended");
    throws(() => revealed.submit("231"), "cannot guess after reveal");
    return { pairs, assertions };
  }

  if (typeof module === "object" && module.exports) {
    try {
      const result = runTests();
      console.log(`PASS: ${result.pairs} exhaustive secret/guess pairs, ${result.assertions} assertions.`);
    } catch (err) {
      console.error("FAIL:", err.message);
      process.exitCode = 1;
    }
  } else {
    root.runMasterMindTests = runTests;
  }
})(typeof globalThis !== "undefined" ? globalThis : this);
