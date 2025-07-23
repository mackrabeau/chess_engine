#include "board.h"
#include "movegenerator.h"
#include "game.h"
#include "move.h"
#include "evaluation.h"
#include "search.h"


#include <iostream>
#include <cassert>

using namespace std;


std::string getRandomMove(const std::string& fen) {
    Game game(fen);
    MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(game.board);

    if (legalMoves.getNumMoves() == 0) {
        return "";
    }
    
    // Randomly select a move from the legal moves
    int randomIndex = rand() % legalMoves.getNumMoves();
    Move randomMove = legalMoves.getMove(randomIndex);
    return randomMove.toString();
}

std::string getBestMove(const std::string& fen) {
    Game game(fen);
    int bestScore = -40000;
    Move bestMove;

    static MovesStruct legalMoves;
    legalMoves = game.moveGen.generateAllLegalMoves(game.board);
    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        game.pushMove(move);
        int score = -alphabeta(-MATE_VALUE, MATE_VALUE, 4, game);
        game.popMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove.toString();
}

std::string makeMove(const std::string& fen, const std::string& moveStr) {
    Game game(fen);
    int fromSquare = (moveStr[0] - 'a')  + (moveStr[1] - '1') * 8; // Convert 'e2' to 4
    int toSquare = (moveStr[2] - 'a') + (moveStr[3] - '1') * 8; // Convert 'e4' to 6

    if (!(game.moveGen.isLegal(fromSquare, toSquare, game.board))) {
        return ""; // Invalid move
    }

    Move move = game.moveGen.readMove(fromSquare, toSquare, game.board);

    Board newBoard = game.makeMove(move);
    return newBoard.toString(); // Assuming Board has a toString() method for FEN representation
}

std::string getGameState(const std::string& fen) {
    Game game(fen);

    GameState state = game.returnGameState(game.board);
    switch (state) {
        case ONGOING: return "ongoing";
        case CHECKMATE: return "checkmate";
        case STALEMATE: return "stalemate";
        case DRAW_REPETITION: return "draw_repetition";
        case DRAW_50_MOVE: return "draw_50_move";
        case DRAW_INSUFFICIENT_MATERIAL: return "draw_insufficient_material";
        default: return "unknown"; // Handle unexpected states
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Missing argument!\n";
        return 1;
    }

    std::string command = argv[1];
    std::string fen = (argc > 2) ? argv[2] : "";
    std::string result;
    
    if (command == "move" && argc > 3) {
        std::string move = argv[3];
        result = makeMove(fen, move);  // Function can be named anything
    } 
    else if (command == "state" && argc > 2) {
        result = getGameState(fen);  // Function can be named anything
    }
    else if (command == "random" || argc == 2) {
        result = getRandomMove(fen);  // Function can be named anything
    } else if (command == "best") {
        result = getBestMove(fen);
    }

    std::cout << result;
    return 0;
}
