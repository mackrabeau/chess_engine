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
    U64 hash;
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

    void reset(){
        board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // Reset to initial state
        updateGameState();
    }


    void pushMove(const Move& move);
    Board makeMove(const Move& move);
    Board makeMove(Board& fromBoard, const Move& move);
    void popMove();

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