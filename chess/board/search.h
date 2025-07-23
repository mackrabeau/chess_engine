#ifndef SEARCH_H
#define SEARCH_H


#include "evaluation.h"


int quiescenceSearch(int alpha, int beta, int depth);
int alphabeta(int alpha, int beta, int depth, Game& game);

#endif // SEARCH_H