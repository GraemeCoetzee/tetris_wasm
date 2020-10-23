hideGameButtons();
hideRestartButton();
hideStopButton();
hideEndGif();
hideFinalScore();

var Module = {
  preRun: [],
  postRun: [],
  print: (function () {
    return function (text) {
      if (arguments.length > 1)
        text = Array.prototype.slice.call(arguments).join(" ");
      console.log(text);
    };
  })(),
  printErr: function (text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(" ");
    console.error(text);
  },
};

function hideGameButtons() {
  $("#left").hide();
  $("#right").hide();
  $("#down").hide();
  $("#rotate").hide();
}

function showGameButtons() {
  $("#left").show();
  $("#right").show();
  $("#down").show();
  $("#rotate").show();
}

function hideRestartButton() {
  $("#restart").hide();
}

function showRestartButton() {
  $("#restart").show();
}

function hideStartButton() {
  $("#start").hide();
}

function showStartButton() {
  $("#start").show();
}

function hideStopButton() {
  $("#stop").hide();
}

function showStopButton() {
  $("#stop").show();
}

function hideStartGif() {
  $("#start-gif").hide();
}

function showStartGif() {
  $("#start-gif").show();
}

function hideEndGif() {
  $("#game-over-gif").hide();
}

function showEndGif() {
  $("#game-over-gif").show();
}

function hideFinalScore() {
  $("#final-score").hide();
}

function showFinalScore() {
  $("#final-score").show();
}

function start() {
  hideFinalScore();
  updateScore();
  hideStartButton();
  showRestartButton();
  showStopButton();
  showGameButtons();
  stopTimer();
  startTimer();
  initializeGame();
  updateGameState();
  hideStartGif();
}

function resetInterface() {
  $("#tetrisField").html("");
  $(".score").html("");
}

function stop() {
  stopTimer();
  showStartButton();
  hideStopButton();
  hideRestartButton();
  hideGameButtons();
  resetInterface();
  showStartGif();
}

function initializeGame() {
  let n = Module.ccall("initializeGame", null, [], []);
}

function reset() {
  hideFinalScore();
  stopTimer();
  startTimer();
  initializeGame();
  updateGameState();
  hideEndGif();
}

function updateGameState() {
  let n = Module.ccall("getGameState", "string", [], []);

  let tetrisField = $("#tetrisField");
  tetrisField.html("").addClass("mx-auto");

  let index = 0;
  for (let i = 0; i < 22; i++) {
    let row = $("<div></div>")
      .addClass("row")
      .addClass("justify-content-center");
    for (let j = 0; j < 12; j++) {
      let wall = n[index] == " " ? false : true;

      let column = $("<div></div>").addClass("col").addClass("block");

      switch (n[index]) {
        case "R":
          column.addClass("colour-red");
          break;
        case "O":
          column.addClass("colour-orange");
          break;
        case "Y":
          column.addClass("colour-yellow");
          break;
        case "G":
          column.addClass("colour-green");
          break;
        case "B":
          column.addClass("colour-blue");
          break;
        case "P":
          column.addClass("colour-purple");
          break;
        case "M":
          column.addClass("colour-pink");
          break;
        case "W":
          column.addClass("colour-wall");
          break;
        default:
          column.addClass("colour-light-grey");
          break;
      }

      row.append(column);
      index++;
    }
    tetrisField.append(row);
  }
  console.log(n);
}

function moveLeft() {
  let n = Module.ccall(
    "movePiece",
    "boolean",
    ["boolean", "number"],
    [false, 0]
  );

  if (n) {
    endGame();
    return;
  }

  updateGameState();
}

function moveRight() {
  let n = Module.ccall(
    "movePiece",
    "boolean",
    ["boolean", "number"],
    [false, 1]
  );

  if (n) {
    endGame();
    return;
  }

  updateGameState();
}

function moveDown() {
  let n = Module.ccall(
    "movePiece",
    "boolean",
    ["boolean", "number"],
    [false, 2]
  );

  if (n) {
    endGame();
    return;
  }

  updateGameState();
}

function updateScore() {
  let score = Module.ccall("getScore", "number", [], []);
  $(".score").html(`Score: ${score}`);
}

function rotate() {
  let n = Module.ccall(
    "movePiece",
    "boolean",
    ["boolean", "number"],
    [false, 3]
  );

  if (n) {
    endGame();
    return;
  }

  updateGameState();
}

let timer;

function startTimer() {
  timer = setInterval(function () {
    let n = Module.ccall(
      "movePiece",
      "boolean",
      ["boolean", "number"],
      [true, 2]
    );

    if (n) {
      endGame();
      return;
    }

    updateGameState();
    updateScore();
  }, 1000);
}

function stopTimer() {
  clearInterval(timer);
}

function endGame() {
  updateGameState();
  stopTimer();
  hideStopButton();
  hideStartGif();
  showEndGif();
  hideGameButtons();
  resetInterface();
  showFinalScore();

  let score = Module.ccall("getScore", "number", [], []);
  $("#final-score").html(`Score: ${score}`);
}
