
#include "search.h"

std::unordered_map<U64, TTEntry> transpositionTable;

int alphabeta(int alpha, int beta, int depth, Game& game){
    int originalAlpha = alpha;
    U64 hash = game.board.getHash();
    
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end() && it->second.depth >= depth) {
        TTEntry& entry = it->second;
        if (entry.flag == TT_EXACT) {
            return entry.score;
        } else if (entry.flag == TT_LOWER && entry.score >= beta) {
            return entry.score;
        } else if (entry.flag == TT_UPPER && entry.score <= alpha) {
            return entry.score;
        }
    }

    if (depth == 0) {
        return evaluateBoard(game.board);
    }

    if (game.returnGameState(game.board) != ONGOING) {
        int score = evaluateBoard(game.board);
        TTEntry entry = {score, depth, TT_EXACT, Move()};
        transpositionTable[hash] = entry;
        return score;
    }

    // Generate legal moves
    MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(game.board);
    
    // no legal moves --> checkmate or stalemate
    if (legalMoves.getNumMoves() == 0) {
        int score = game.board.isInCheck() ? -MATE_VALUE + depth : STALEMATE_VALUE;
        TTEntry entry = {depth, score, TT_EXACT, Move()};
        transpositionTable[hash] = entry;
        return score;
    }

    int maxScore = -MATE_VALUE - 1; // worst possible score
    Move bestMove;

    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        game.pushMove(move);
        int score = -alphabeta(-beta, -alpha, depth - 1, game);
        game.popMove(); // Undo the move

        if (score > maxScore) {
            maxScore = score;
            bestMove = move;
        }

        alpha = std::max(alpha, score);

        if (alpha >= beta) {
            bestMove = move;
            break; // Prune remaining moves
        }
    }

    // Store in transposition table with correct flag
    TTEntry entry;
    entry.depth = depth;
    entry.score = maxScore;
    entry.bestMove = bestMove;
    
    if (maxScore <= originalAlpha) {
        entry.flag = TT_UPPER; // Upper bound
    } else if (maxScore >= beta) {
        entry.flag = TT_LOWER; // Lower bound  
    } else {
        entry.flag = TT_EXACT; // Exact score
    }
    
    transpositionTable[hash] = entry;
    return maxScore;
}


int quiescenceSearch(int alpha, int beta, int depth) {
    // This function should implement the quiescence search logic
    // For now, we will return a placeholder value
    return 0;
}