#include "board.h"
#include "movegenerator.h"
#include "game.h"
#include "move.h"


#include <iostream>
#include <cassert>


void test_checkmate() {
    // Fool's mate: White is checkmated
    Board board("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 0 3");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct legalMoves = moveGen.generateAllLegalMoves(board);
    assert(legalMoves.getNumMoves() == 0 && "Checkmate: should be 0 legal moves");
}

void test_stalemate() {
    // Black to move, stalemate
    Board board("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    assert(legalMoves.getNumMoves() == 0 && "Stalemate: should be 0 legal moves");
}

void test_en_passant_legality() {
    // En passant is legal
    Board board("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);

    bool foundEnPassant = false;
    for (int i = 0; i < legalMoves.getNumMoves(); ++i)
        if (legalMoves.getMove(i).getMove() & FLAG_EP_CAPTURE) foundEnPassant = true;
    assert(foundEnPassant && "En passant should be legal");
}

void test_en_passant_illegality() {
    // En passant is not legal (would leave king in check)
    Board board("8/8/8/3pP3/8/8/8/R3K2R w KQ - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    for (int i = 0; i < legalMoves.getNumMoves(); ++i){
        assert(!(legalMoves.getMove(i).getFlags() == FLAG_EP_CAPTURE) && "En passant should NOT be legal");
    }
}

void test_castling_rights() {
    // Both sides can castle
    Board board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    bool foundKingCastle = false, foundQueenCastle = false;
    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        if ((move.getFlags() & FLAG_KING_CASTLE) && !(move.getFlags() & FLAG_DOUBLE_PAWN_PUSH)) foundKingCastle = true;
        if ((move.getFlags() & FLAG_KING_CASTLE) && (move.getFlags() & FLAG_DOUBLE_PAWN_PUSH)) foundQueenCastle = true;
    }
    assert(foundKingCastle && "King-side castling should be legal");
    assert(foundQueenCastle && "Queen-side castling should be legal");
}

void test_pawn_promotions() {
    // White pawn ready to promote
    Board board("8/P7/8/8/8/8/8/8 w - - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    bool foundPromotion = false;
    for (int i = 0; i < legalMoves.getNumMoves(); ++i)
        if (legalMoves.getMove(i).getMove() & FLAG_PROMOTION) foundPromotion = true;
    assert(foundPromotion && "Pawn promotion should be detected");
}

void test_pinned_piece_cannot_move() {
    // Bishop is pinned, cannot move
    Board board("4k3/8/8/8/8/8/4B3/4K3 w - - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        int from = move.getFrom();
        if (from == 11) // d2
            assert(false && "Pinned bishop should not be able to move");
    }
}

void test_king_cannot_move_into_check() {
    // King surrounded by enemy pawns, cannot move into check
    Board board("8/8/8/3pKp2/8/8/8/8 w - - 0 1");
    MoveGenerator moveGen(MoveTables::instance());
    MovesStruct  legalMoves = moveGen.generateAllLegalMoves(board);
    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        int from = move.getFrom();
        int to = move.getTo();
        if (from == 36 && (to == 28)) // e5 to d4 or f4
            assert(false && "King should not be able to move into check");
    }
}

