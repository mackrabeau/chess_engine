#ifndef SEARCH_H
#define SEARCH_H

#include "evaluation.h"

const int MATE_VALUE = 30000;
const int STALEMATE_VALUE = 0;

int quiescenceSearch(int alpha, int beta, int depth);
int alphabeta(int alpha, int beta, int depth, Game& game);

#endif // SEARCH_H