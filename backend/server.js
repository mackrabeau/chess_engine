
const express = require('express');
const { spawn } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 3001;


class ChessEngine{
  constructor(enginePath) {
    this.engine = spawn(enginePath);
    this.pendingRequests = new Map();
    this.requestId = 0;
    this.currentDepth = 0;

    console.log("Engine Started...");

    this.engine.stdout.on('data', (data) => {
      const response = data.toString().trim();
      console.log("Engine Output:", response);
      this.handleResponse(response);
    });
    
    this.engine.stderr.on('data', (data) => {
        const message = data.toString().trim();
        // console.log(`Raw stderr: "${message}"`);
        
        // Filter out search stats, engine info, and debug messages
        if (message.startsWith('STATS:') || 
            message.startsWith('Engine Info:') ||
            message.startsWith('TT initialized:') ||
            message.startsWith('DEPTH:') ||
            message.startsWith('Current Search Depth:') ||
            message.startsWith('=== ') ||  // Debug section headers
            message.startsWith('Available moves:') ||
            message.startsWith('Move ') ||
            message.startsWith('Static evaluation:') ||
            message.startsWith('Quiescence result:') ||
            message.startsWith('Difference:') ||
            message.startsWith('Available captures:') ||
            message.startsWith('Capture ') ||
            message.startsWith('After sorting:') ||
            message.startsWith('Found move:') ||
            message.startsWith('Eval before:') ||
            message.startsWith('Eval after:') ||
            message.startsWith('Change:') ||
            message.includes('Search depth') ||
            message.match(/^\d+$/)) { // Single numbers (likely depth indicators)
            console.log(`Engine Info: ${message}`);
        } else {
            console.error(`Engine Error: ${message}`);
        }
    });    this.engine.on('close', (code) => {
        console.log('Engine process exited with code:', code);
    });
    
    this.engine.on('error', (error) => {
        console.error('Engine process error:', error);
    });
  }
  
  sendCommand(command, args) {
    return new Promise((resolve, reject) => {
        const id = ++this.requestId;
        const fullCommand = `${id} ${command}`;
        console.log('Sending to engine:', fullCommand);
        
        // Timeout after 60 seconds
        const timeout = setTimeout(() => {
            if (this.pendingRequests.has(id)) {
                this.pendingRequests.delete(id);
                reject(new Error('Engine timeout'));
            }
        },  60000);

        this.pendingRequests.set(id, {
            resolve: (result) => {
                clearTimeout(timeout);
                resolve(result);
            },
            reject: (err) => {
                clearTimeout(timeout);
                reject(err);
            }
        });
        
        this.engine.stdin.write(fullCommand + '\n');
    });
  }

  handleResponse(response) {
      // Parse response: "requestId result"
      const spaceIndex = response.indexOf(' ');
      if (spaceIndex === -1) {
          console.error('Invalid response format:', response);
          return;
      }
      
      const id = parseInt(response.substring(0, spaceIndex));
      const result = response.substring(spaceIndex + 1);
      
      const request = this.pendingRequests.get(id);
      if (request) {
          this.pendingRequests.delete(id);
          request.resolve(result);
      } else {
          console.error('No pending request found for ID:', id);
      }
  }

  // Convenience methods
  async getBestMove() {
    this.currentDepth = 0; // Reset depth before each search
    return this.sendCommand(`best`);
  }
  
  async makeMove(move) {
      return this.sendCommand(`move ${move}`);
  }

  async getGameState() {
      return this.sendCommand(`state`);
  }

  async getEval(){
    return this.sendCommand('eval');
  }

  async getPosition(){
    return this.sendCommand('position');
  }
  
  async reset() {
    this.currentDepth = 0; // Reset depth tracking
    return this.sendCommand('reset');
  }

  getCurrentDepth() {
    return this.currentDepth;
  }
  
  shutdown() {
      console.log('Shutting down chess engine...');
      this.engine.stdin.write('quit\n');
      setTimeout(() => {
          this.engine.kill();
      }, 1000);
  }
}


// initialize the chess engines
const enginePath = path.join(__dirname, 'engine', 'engine'); // compiled C++ executable
const chessEngine = new ChessEngine(enginePath);

// graceful shutdown
process.on('SIGINT', () => {
    console.log('Received SIGINT, shutting down...');
    chessEngine.shutdown();
    process.exit(0);
});

process.on('SIGTERM', () => {
    console.log('Received SIGTERM, shutting down gracefully...');
    chessEngine.shutdown();
    process.exit(0);
});

app.use(cors());
app.use(express.json());

// error handling middleware
const handleEngineError = (error, res, operation) => {
    console.error(`Engine error during ${operation}:`, error);
    res.status(500).json({ 
        error: "Engine error", 
        operation: operation,
        message: error.message 
    });
};

app.post('/api/getBestMove', async (req, res) => {
    try {
        console.log("Requesting best move...");
        const bestMove = await chessEngine.getBestMove();
        console.log("Best move returned:", bestMove);
        
        res.json({ bestMove });
    } catch (error) {
        handleEngineError(error, res, 'getBestMove');
    }
});

app.post('/api/makeMove', async (req, res) => {
    const { from, to, promoPiece } = req.body;

    if (!from || !to) {
        return res.status(400).json({
            error: "From and to parameters required"
        });
    }
    
    const move = from + to + promoPiece;
    
    try {
        console.log("Making move:", move);
        const newPosition = await chessEngine.makeMove(move);
        console.log("Engine returned new position:", newPosition);
        
        res.json({ newPosition });
    } catch (error) {
        handleEngineError(error, res, 'makeMove');
    }
});

// Assuming the engine returns one of the following states:
// "ongoing"
// "checkmate"
// "stalemate"
// "draw_repetition"
// "draw_50_move"
// "draw_insufficient_material"

// Get game state
app.post('/api/getGameState', async (req, res) => {
    try {
        console.log("Checking game state...");
        const gameState = await chessEngine.getGameState();
        console.log("Game state returned:", gameState);
        
        // Check if game is over
        const isGameOver = gameState !== "ongoing";
        
        res.json({ 
            isGameOver, 
            result: gameState 
        });
    } catch (error) {
        handleEngineError(error, res, 'getGameState');
    }
});

// Reset engine (clear transposition table)
app.post('/api/reset', async (req, res) => {
    try {
        console.log("Resetting chess engine...");
        const result = await chessEngine.reset();
        console.log("Engine reset:", result);
        
        res.json({ 
            status: 'reset',
            message: 'Engine and transposition table cleared'
        });
    } catch (error) {
        handleEngineError(error, res, 'reset');
    }
});

app.post('/api/getEval', async (req, res) => {
    try {
        console.log("Requesting position evaluation...");
        const evaluation = await chessEngine.sendCommand('eval');
        console.log("Evaluation returned:", evaluation);
        
        // Parse the evaluation number from the response
        const evalValue = parseInt(evaluation) || 0;
        
        res.json({ evaluation: evalValue });
    } catch (error) {
        handleEngineError(error, res, 'getEval');
    }
});

// Get current position from engine
app.post('/api/getPosition', async (req, res) => {
    try {
        console.log("Requesting current position...");
        const position = await chessEngine.getPosition();
        console.log("Engine returned position:", position);
        
        res.json({ position: position });
    } catch (error) {
        handleEngineError(error, res, 'getPosition');
    }
});

// Get current search depth
app.get('/api/searchDepth', (req, res) => {
    try {
        const depth = chessEngine.getCurrentDepth();
        res.json({ depth });
    } catch (error) {
        res.status(500).json({ error: 'Failed to get search depth' });
    }
});

// Health check endpoint
app.get('/api/health', (req, res) => {
    res.json({ 
        status: 'ok', 
        engine: 'running',
        timestamp: new Date().toISOString()
    });
});

// Get engine stats (maybe add TT hit rate, etc.)
app.get('/api/stats', async (req, res) => {
    try {
        // You could add a 'stats' command to your engine
        const stats = await chessEngine.sendCommand('stats');
        res.json({ stats });
    } catch (error) {
        // If stats not implemented, return basic info
        res.json({ 
            status: 'Engine running',
            uptime: process.uptime(),
            message: 'Detailed stats not implemented'
        });
    }
});


// error handling for undefined routes
app.use((req, res) => {
    res.status(404).json({ error: 'Endpoint not found' });
});

app.use((error, req, res, next) => {
    console.error('Unhandled error:', error);
    res.status(500).json({ error: 'Internal server error' });
});


// start server
app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});

