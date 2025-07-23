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
g++ -std=c++11 game.cpp board.cpp movegenerator.cpp movetables.cpp search.cpp evaluation.cpp perft.cpp -o ../../backend/engine/perft
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
- Watch the binary move representation update in real-time

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
- **Move ordering** for better pruning efficiency
- **Quiescence search** for tactical positions
- **Iterative deepening** for time management

## 🎯 Future Improvements

- [ ] **Opening book** integration
- [ ] **Endgame tablebase** support
- [ ] **Transposition tables** for faster search
- [ ] **UCI protocol** compatibility
- [ ] **Time management** and tournament play
- [ ] **Position evaluation** improvements
- [ ] **Parallel search** with multiple threads

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 License

This project is open source and available under the [MIT License](LICENSE).

## 🎮 Usage Examples

### Engine Commands
```bash
# Get a random move for the starting position
./engine random "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

# Get the best move
./engine best "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

# Make a move
./engine move "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "e2e4"

# Check game state
./engine state "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

## 🐛 Known Issues

- Promotion currently defaults to Queen (user choice not implemented)
- No support for draw by repetition detection yet
- Time controls not implemented for tournament play

---

**Built with ❤️ by [mackrabeau](https://github.com/mackrabeau)**
