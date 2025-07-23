#ifndef GAME_H
#define GAME_H

#include "movegenerator.h"
#include "types.h"
#include "movetables.h"
#include "board.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

struct BoardState {
    U64 pieceBB[8];
    U16 gameInfo;
};

enum GameState {
    ONGOING,
    CHECKMATE,
    STALEMATE,
    DRAW_REPETITION,
    DRAW_50_MOVE,
    DRAW_INSUFFICIENT_MATERIAL
};

class Game {

public:
    Board board;
    GameState state;
    MoveGenerator moveGen;

    Game(const std::string& initialFEN) 
        : board(initialFEN), moveGen(MoveTables::instance()) { updateGameState(); };


    void doMove(const Move& move);
    Board makeMove(const Move& move);
    Board makeMove(Board& fromBoard, const Move& move);
    void undoMove();

    void updateGameState();

    // make this static??
    GameState returnGameState(Board& board);

    bool isThreefoldRepetition() const {return false;};
    bool isFiftyMoveRule() const {return false;};
    bool isInsufficientMaterial() const {return false;};


private:
    std::vector<BoardState> history; // to store previous board states

};

#endif // GAME_H;