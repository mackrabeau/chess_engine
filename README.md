# Chess Engine

A full-stack chess engine implementation featuring a C++ backend with alpha-beta search and a React frontend with an interactive chess board.

## 🚀 Features

### Chess Engine Core
- **Complete move generation** with legal move validation
- **Alpha-beta search algorithm** with configurable depth
- **Full chess rules support**:
  - Castling (kingside and queenside)
  - En passant captures
  - Pawn promotion (all piece types)
  - Check, checkmate, and stalemate detection
  - Threefold repetition detection
  - 50-move rule implementation
- **FEN string parsing** and generation
- **Performance testing** with perft functionality
- **Binary move representation** with detailed visualization

### Web Interface
- **Interactive chess board** using react-chessboard
- **Real-time move validation** and highlighting
- **Computer opponent** with random and best-move AI
- **En passant square highlighting** for debugging
- **Binary move display** showing internal move representation
- **Game state tracking** (ongoing, checkmate, stalemate, draws)
- **Evaluation bar** showing current position strength
- **Move history** with notation and evaluation scores
- **Responsive layout** that works on different screen sizes
- **"Thinking..." indicator** while the engine searches

## 📁 Project Structure

```
chess/
├── README.md
├── backend/
│   ├── package.json         # Node.js dependencies
│   ├── server.js           # Express API server
│   └── engine/             # Compiled C++ executables
├── frontend/
│   ├── package.json        # React dependencies
│   ├── src/
│   │   └── components/
│   │       └── ChessBoard.tsx  # Main chess component
│   ├── public/
│   └── vite.config.ts
└── chess/board/            # C++ source code
    ├── engine.cpp          # Main engine interface
    ├── board.cpp           # Board representation
    ├── board.h
    ├── movegenerator.cpp   # Move generation logic
    ├── movegenerator.h
    ├── search.cpp          # Alpha-beta search
    ├── search.h
    ├── evaluation.cpp      # Position evaluation
    ├── evaluation.h
    ├── game.cpp           # Game state management
    ├── game.h
    ├── movetables.cpp     # Pre-computed move tables
    ├── movetables.h
    ├── types.h            # Type definitions
    ├── move.h             # Move representation
    └── perft.cpp          # Performance testing
```

## 🛠️ Building and Installation

### Prerequisites
- **C++11** compiler (g++ or clang)
- **Node.js** (v16 or higher)
- **npm** or **yarn**

### Build the C++ Engine

Navigate to the chess engine source directory:
```bash
cd chess/board
```

Compile the main engine:
```bash
g++ -std=c++11 game.cpp board.cpp movegenerator.cpp movetables.cpp search.cpp evaluation.cpp engine.cpp -o ../../backend/engine/engine
```

Compile the performance tester (optional):
```bash
g++ -std=c++11 game.cpp board.cpp movegenerator.cpp movetables.cpp search.cpp evaluation.cpp perft.cpp -o perft
```

### Setup Backend API

```bash
cd backend
npm install
```

### Setup Frontend

```bash
cd frontend
npm install
```

## 🚀 Running the Application

### 1. Start the Backend API Server
```bash
cd backend
node server.js
```
The API server will run on `http://localhost:3001`

### 2. Start the Frontend Development Server
```bash
cd frontend
npm run dev
```
The web interface will be available at `http://localhost:5173`

### 3. Play Chess!
- Open your browser to the frontend URL
- Make moves by dragging pieces on the board
- The engine will respond with its best move

## 🔧 API Endpoints

The backend provides these REST endpoints:

- `POST /api/getRandomMove` - Get a random legal move
- `POST /api/getBestMove` - Get the best move using alpha-beta search
- `POST /api/makeMove` - Apply a move and get the new position
- `POST /api/getGameState` - Check if the game is over

Each endpoint accepts a FEN string representing the current position.

## 🧪 Testing and Performance

### Run Perft Tests
```bash
./backend/engine/perft [depth] [fen]
```

Example:
```bash
./backend/engine/perft 4 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

### Expected Perft Results (Starting Position)
| Depth | Nodes       |
|-------|-------------|
| 1     | 20          |
| 2     | 400         |
| 3     | 8,902       |
| 4     | 197,281     |
| 5     | 4,865,609   |

## 🏗️ Technical Details

### Move Representation
Moves are encoded as 16-bit integers:
- **Bits 0-5**: Target square (0-63)
- **Bits 6-11**: Source square (0-63)  
- **Bits 12-15**: Move flags (piece type, special moves)

### Board Representation
- **Bitboards**: 64-bit integers representing piece positions
- **8x8 array mapping**: Square indices 0-63 (a1=0, h8=63)
- **Compact game state**: Castling rights, en passant, turn stored in single 16-bit integer

### Search Algorithm
- **Alpha-beta pruning** with configurable depth
- **Move ordering** for better pruning efficiency (MVV-LVA, killer moves)
- **Quiescence search** for tactical positions
- **Iterative deepening** for time management
- **Transposition tables** for storing and reusing previous search results
- **Time-based search cutoff** for responsive play

## 🎯 Future Improvements

- [ ] **Opening book** integration
- [ ] **Endgame tablebase** support
- [ ] **UCI protocol** compatibility
- [ ] **Position evaluation** improvements
- [ ] **Parallel search** with multiple threads

## 🎮 Usage Examples

### Running the Engine

First, compile and start the engine:
```bash
cd chess/board
g++ -std=c++11 -o engine [source files]
./engine
```

The engine accepts commands via standard input. Each command starts with a request ID (any number) followed by the command and its parameters.

### Basic Commands
```bash
# Set a position using FEN notation
1 engine set_position rnbqkbnr/pppppppp/8/8/8/5P2/PPPPP1PP/RNBQKBNR b KQkq - 0 1

# Get the best move for current position
2 engine best

# Make a move (from e2 to e4)
3 engine move e2e4

# Check current game state (ongoing, checkmate, draws, etc.)
4 engine state

# Get current position evaluation
5 engnine eval

# Get current position as FEN string
6 engine position

# Reset to starting position
7 engine reset

# Display board in terminal
8 engine print

# Quit the engine
9 engine quit
```

### Debug Commands
```bash
# Get transposition table statistics
10 engine tt_stats

# Debug best move evaluation
11 engine debug_best_move
```

## 🐛 Known Issues

- Promotion supports piece selection ('n', 'b', 'r', 'q') but defaults to Queen in UI

---

**Built by [mackrabeau](https://github.com/mackrabeau)**
