#ifndef MOVETABLES_H
#define MOVETABLES_H

#include "types.h"


class MoveTables {
public:
    U64 kingBB[64];
    U64 knightBB[64];
    U64 pawnMovesBB[2][64];
    U64 pawnMovesCapturesBB[2][64];

    static MoveTables& instance() {
        static MoveTables instance;
        return instance;
    }

    void init();

private:
    MoveTables() { init(); } // private constructor to enforce singleton pattern
    MoveTables(const MoveTables&) = delete; // prevent copying
    MoveTables& operator=(const MoveTables&) = delete; // prevent assignment

    void generateKingMoves();
    void generateKnightMoves();
    void generatePawnMoves();

    void generateSlidingPieceMoves();

    void updateBitboard(U64 &bitboard, int row, int col, int move[2]);
    void updatePawnMoves(U64 &whitePawnMoves, U64 &blackPawnMoves, int row, int col);
    void updatePawnCaptures(U64 &whiteCaptures, U64 &blackCaptures, int row, int col);

};

#endif // MOVETABLES_H