#ifndef SEARCH_H
#define SEARCH_H

#include "evaluation.h"
#include <unordered_map>

struct TTEntry {
    int score;
    int depth;
    int flag; // 0: exact, 1: lower bound, 2: upper bound
    Move bestMove;
};

const int TT_EXACT = 0;
const int TT_LOWER = 1;
const int TT_UPPER = 2;

const int MATE_VALUE = 30000;
const int STALEMATE_VALUE = 0;

extern std::unordered_map<U64, TTEntry> transpositionTable;

int quiescenceSearch(int alpha, int beta, int depth);
int alphabeta(int alpha, int beta, int depth, Game& game);

#endif // SEARCH_H