(function () {
  "use strict";
  const MM = window.MasterMind;
  const slots = Array.from(document.querySelectorAll(".slot"));
  const palette = Array.from(document.querySelectorAll(".color-option"));
  const board = document.getElementById("history");
  const attemptLabel = document.getElementById("attempts");
  const status = document.getElementById("status");
  const submitButton = document.getElementById("submit-guess");
  const newButton = document.getElementById("new-game");
  const revealButton = document.getElementById("reveal-secret");
  const secretInput = document.getElementById("demo-secret");
  const secretReveal = document.getElementById("secret-reveal");
  const selected = [null, null, null];
  let currentSlot = 0;
  let game = MM.createGame();

  function makePeg(color, small = false) {
    const peg = document.createElement("span");
    peg.className = `peg peg-${color}${small ? " peg-small" : ""}`;
    peg.setAttribute("aria-label", ["", "green", "red", "blue"][color]);
    return peg;
  }
  function setStatus(message, variant = "info") {
    status.textContent = message;
    status.dataset.variant = variant;
  }
  function drawSelection() {
    slots.forEach((slot, index) => {
      slot.replaceChildren();
      slot.classList.toggle("active", index === currentSlot && game.status === "playing");
      slot.setAttribute("aria-label", `Position ${index + 1}: ${selected[index] || "empty"}`);
      if (selected[index]) slot.appendChild(makePeg(selected[index]));
      else {
        const placeholder = document.createElement("span");
        placeholder.className = "slot-placeholder";
        placeholder.textContent = String(index + 1);
        slot.appendChild(placeholder);
      }
    });
    const playable = game.status === "playing";
    submitButton.disabled = !playable || selected.some(value => value === null);
    palette.forEach(button => button.disabled = !playable);
    revealButton.disabled = !playable;
  }
  function chooseColor(color) {
    if (game.status !== "playing") return;
    selected[currentSlot] = color;
    const nextEmpty = selected.findIndex((value, i) => i > currentSlot && value === null);
    currentSlot = nextEmpty === -1 ? (currentSlot + 1) % MM.LENGTH : nextEmpty;
    drawSelection();
  }
  function addHistory(guess, exact, approx, won) {
    const item = document.createElement("li");
    item.className = `history-item${won ? " is-win" : ""}`;
    const ordinal = document.createElement("span");
    ordinal.className = "ordinal";
    ordinal.textContent = `#${game.attempts}`;
    const dots = document.createElement("span");
    dots.className = "guess-dots";
    guess.forEach(color => dots.appendChild(makePeg(color, true)));
    const score = document.createElement("span");
    score.className = "score";
    score.textContent = `${exact} exact · ${approx} approx`;
    item.append(ordinal, dots, score);
    if (board.firstElementChild?.classList.contains("empty-state")) board.replaceChildren();
    board.prepend(item);
  }
  function resetSelection() {
    selected.fill(null);
    currentSlot = 0;
    drawSelection();
  }
  function startGame() {
    try {
      const entered = secretInput.value.trim();
      game = entered ? MM.createGame(entered) : MM.createGame();
      board.replaceChildren();
      const empty = document.createElement("li");
      empty.className = "empty-state";
      empty.textContent = "Your guesses will appear here.";
      board.appendChild(empty);
      attemptLabel.textContent = "0";
      secretReveal.hidden = true;
      secretReveal.textContent = "";
      setStatus(entered ? "Demo game started with the chosen hidden code." : "New game started. Good luck!");
      resetSelection();
    } catch (err) {
      setStatus(err.message, "error");
    }
  }
  function submitGuess() {
    if (game.status !== "playing" || selected.some(v => v === null)) return;
    try {
      const result = game.submit(selected);
      addHistory(result.guess, result.exact, result.approx, result.won);
      attemptLabel.textContent = String(game.attempts);
      if (result.won) {
        setStatus(`You cracked the code in ${result.attempts} ${result.attempts === 1 ? "attempt" : "attempts"}!`, "success");
        secretReveal.textContent = `Secret: ${result.guess.join(" · ")}`;
        secretReveal.hidden = false;
        drawSelection();
      } else {
        setStatus(`${result.exact} exact and ${result.approx} approximate. Keep going!`);
        resetSelection();
      }
    } catch (err) {
      setStatus(err.message, "error");
    }
  }
  function revealGame() {
    if (game.status !== "playing") return;
    const secret = game.reveal();
    secretReveal.textContent = `Secret: ${secret.join(" · ")}`;
    secretReveal.hidden = false;
    setStatus("Secret revealed. Start a new game to play again.");
    drawSelection();
  }

  slots.forEach((button, index) => button.addEventListener("click", () => {
    currentSlot = index;
    drawSelection();
  }));
  palette.forEach(button => button.addEventListener("click", () => chooseColor(Number(button.dataset.color))));
  submitButton.addEventListener("click", submitGuess);
  newButton.addEventListener("click", startGame);
  revealButton.addEventListener("click", revealGame);
  document.addEventListener("keydown", event => {
    if (event.target instanceof HTMLInputElement) return;
    if (["1", "2", "3"].includes(event.key)) {
      event.preventDefault();
      chooseColor(Number(event.key));
    } else if (event.key === "Enter") {
      event.preventDefault();
      submitGuess();
    } else if (event.key === "Backspace" && game.status === "playing") {
      event.preventDefault();
      selected[currentSlot] = null;
      drawSelection();
    }
  });
  drawSelection();
})();
