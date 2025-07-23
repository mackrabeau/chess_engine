
#include "search.h"

int alphabeta(int alpha, int beta, int depth, Game& game){
    if (depth == 0) {
        return evaluateBoard(game.board);
    }

    // Generate legal moves
    MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(game.board);
    
    if (legalMoves.getNumMoves() == 0) {
        // If no legal moves, return a score based on whether the player is in check
        return game.board.isInCheck() ? -MATE_VALUE + depth : STALEMATE_VALUE; // Checkmate or stalemate
    }

    int maxScore = -MATE_VALUE - 1; // worst possible score
    
    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {

        game.pushMove(legalMoves.getMove(i));
        int score = -alphabeta(-beta, -alpha, depth - 1, game);
        game.popMove(); // Undo the move

        maxScore = std::max(maxScore, score);
        alpha = std::max(alpha, score);

        if (alpha >= beta) {
            break; // Prune remaining moves
        }
    }

    return maxScore;
}


int quiescenceSearch(int alpha, int beta, int depth) {
    // This function should implement the quiescence search logic
    // For now, we will return a placeholder value
    return 0;
}