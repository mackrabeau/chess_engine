
const express = require('express');
const { execFile } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 3001;

const enginePath = path.join(__dirname, 'engine', 'engine'); // compiled C++ executable


app.use(cors());
app.use(express.json());

app.post('/api/getRandomMove', (req, res) => {
  const fen = req.body.fen;
  execFile(enginePath, ['random', fen], (error, stdout, stderr) => {
    if (error) {
      console.error("Engine error:", error);
      return res.status(500).send("Engine error");
    }

    const randomMove = stdout.trim();
    console.log("Requesting random move for FEN:", fen);
    console.log("Random move returned:", randomMove);

    res.json({ randomMove });
  });
});


app.post('/api/getBestMove', (req, res) => {
  const fen = req.body.fen;
  execFile(enginePath, ['best', fen], (error, stdout, stderr) => {
    if (error) {
      console.error("Engine error:", error);
      return res.status(500).send("Engine error");
    }

    const bestMove = stdout.trim();
    console.log("Requesting best move for FEN:", fen);
    console.log("Best move returned:", bestMove);

    res.json({ bestMove });
  });
});

app.post('/api/makeMove', (req, res) => {
    const { fen, from, to} = req.body;
    const move = from + to;
    console.log("Making move:", move, "from FEN:", fen);
    execFile(enginePath, ['move', fen, move], (error, stdout, stderr) => {
        console.log("engine returned :", stdout.trim(), "\n");
        if (error) {
            console.error("Engine error:", error);
            return res.status(500).send("Engine error");
        }
        
        const newPosition = stdout.trim();
        res.json({ newPosition });
    });
});

app.post('/api/getGameState', (req, res) => {
    const { fen } = req.body;

    execFile(enginePath, ['state', fen], (error, stdout, stderr) => {
        if (error) {
            return res.status(500).send({ error: "Engine error"});
        }

        // Assuming the engine returns one of the following states:
        // "ongoing"
        // "checkmate"
        // "stalemate"
        // "draw_repetition"
        // "draw_50_move"
        // "draw_insufficient_material"

        const gameState = stdout.trim();
        const isGameOver = gameState !== "ongoing";
        res.json({ isGameOver, result: gameState });
    });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});


