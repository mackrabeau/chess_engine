
#include "search.h"

int alphabeta(int alpha, int beta, int depth, Game& game){
    if (depth == 0) {
        return evaluateBoard(game.board);
    }

    // Generate legal moves
    MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(game.board);
    int maxScore = -10000;
    
    if (legalMoves.getNumMoves() == 0) {
        // If no legal moves, return a score based on whether the player is in check
        return game.board.isInCheck() ? -10000 : 0; // Checkmate or stalemate
    }

    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Game newGame = game; // Create a copy of the game state
        newGame.makeMove(legalMoves.getMove(i));

        int score = -alphabeta(-beta, -alpha, depth - 1, newGame);
        maxScore = std::max(maxScore, score);
        alpha = std::max(alpha, score);

        if (alpha >= beta) {
            break; // Beta cut-off
        }
    }

    return maxScore;
}


int quiescenceSearch(int alpha, int beta, int depth) {
    // This function should implement the quiescence search logic
    // For now, we will return a placeholder value
    return 0;
}