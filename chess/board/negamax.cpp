#include "negamax.h"



int negaMax(const Board& board, int depth, int alpha, int beta, Color maximizingPlayer) {
    if (depth == 0) {
        return evaluate_board(board);
    }

    int maxEval = std::numeric_limits<int>::min();
    for (const auto& move : generate_legal_moves(board, maximizingPlayer)) {
        Board newBoard = board;
        make_move(newBoard, move);
        int eval = -negaMax(newBoard, depth - 1, -beta, -alpha, get_opponent_color(maximizingPlayer));
        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);
        if (beta <= alpha) {
            break;
        }
    }
    return maxEval;
}
