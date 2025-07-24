#include "board.h"
#include "movegenerator.h"
#include "game.h"
#include "move.h"
#include "evaluation.h"
#include "search.h"

#include <iostream>
#include <sstream>

using namespace std;

int main() {
    MoveTables::instance().init(); // Initialize once
    Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // Initial state

    std::string line;
    while (std::getline(std::cin, line)) {

        std::istringstream iss(line);

        std::string requestId, command;
        iss >> requestId >> command;

        if (command == "quit") {
            break;

        } else if (command == "reset") {
            game.reset();
            std::cout << requestId << " ok" << std::endl;

        } else if (command == "best") {
            int bestScore = -40000;
            Move bestMove;

            MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(game.board);
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

            std::cout << requestId << " " << bestMove.toString() << std::endl;

        } else if (command == "move") {
            std::string moveStr;
            iss >> moveStr;

            if (moveStr.length() < 4) {
                std::cout << requestId << " error: invalid move string" << std::endl;
                continue;
            }

            int fromSquare = (moveStr[0] - 'a') + (moveStr[1] - '1') * 8;
            int toSquare = (moveStr[2] - 'a') + (moveStr[3] - '1') * 8;

            if (!(game.moveGen.isLegal(fromSquare, toSquare, game.board))) {
                std::cout << requestId << " error: illegal move" << std::endl;
                continue;
            }

            Move move = game.moveGen.readMove(fromSquare, toSquare, game.board);
            game.pushMove(move);

            std::cout << requestId << " " << game.board.toString() << std::endl;

        } else if (command == "state") {
            GameState state = game.returnGameState(game.board);
            std::string gameStateStr;
            switch (state) {
                case ONGOING: gameStateStr = "ongoing"; break;
                case CHECKMATE: gameStateStr = "checkmate"; break;
                case STALEMATE: gameStateStr = "stalemate"; break;
                case DRAW_REPETITION: gameStateStr = "draw_repetition"; break;
                case DRAW_50_MOVE: gameStateStr = "draw_50_move"; break;
                case DRAW_INSUFFICIENT_MATERIAL: gameStateStr = "draw_insufficient_material"; break;
                default: gameStateStr = "unknown"; break;
            }

            std::cout << requestId << " " << gameStateStr << std::endl;

        } else {
            std::cout << requestId << " error: unknown command" << std::endl;
        }

        std::cout.flush(); // Ensure Node receives output immediately
    }

    return 0;
}
