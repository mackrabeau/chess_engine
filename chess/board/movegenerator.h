#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "types.h"
#include "movetables.h"
#include "move.h"
#include "board.h"

typedef uint64_t U64;
typedef uint16_t U16;
typedef uint8_t U8;

#include <bitset>

#include <iostream>
#include <string>

using namespace std;

class Board; // forward declaration

class MoveGenerator {
private: 

    U64 getBishopAttacks(U64 occupancy, int square);
    U64 getRookAttacks(U64 occupancy, int square);

    void generateKingMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves); 
    void generatePawnMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves); 
    void generateBishopMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves); 
    void generateKnightMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves);
    void generateRookMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves); 
    void generateQueenMovesForSquare(const Board& board, int square, MovesStruct& pseudoMoves);

    const MoveTables& tables; // reference to move tables

public:

    MoveGenerator(const MoveTables& tables) : tables(tables) {}

    MovesStruct generateAllLegalMoves(const Board& board);
    void generateMoves(enumPiece pieceType, const Board& board, int square, MovesStruct& pseudoMoves);    // generates bitmap of all legal moves for that pieces

    void addMovesToStruct(MovesStruct& pseudoMoves, const Board& board, int square, U64 moves); 

    U64 attackedBB(const Board& board, U8 enemyColour); // checks if square is attacked by enemy pieces

    Move readMove(U8 from, U8 to, const Board& board);  // returns move int
    bool isLegal(U8 from, U8 to, Board& board);
    void displayBitboard(U64 bitboard, int square, char symbol) const;

};

#endif // MOVEGENERATOR_H
