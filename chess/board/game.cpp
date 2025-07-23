#include "game.h"
#include "movegenerator.h"
#include "move.h"

using namespace std;

// doMove: modifies the board in place
void Game::doMove(const Move& move) {
    // Save the current board state to history
    BoardState prev;
    memcpy(prev.pieceBB, board.pieceBB, 8 * sizeof(U64));
    prev.gameInfo = board.gameInfo;
    history.push_back(prev);

    Board::applyMove(move, board.pieceBB, board.gameInfo, board);
}

void Game::undoMove() {
    if (history.empty()) return; // Nothing to undo

    BoardState prev = history.back();
    history.pop_back();

    memcpy(board.pieceBB, prev.pieceBB, 8 * sizeof(U64));
    board.gameInfo = prev.gameInfo;
}

// makeMove: returns a new Board
Board Game::makeMove(const Move& move) {
    U64 newPieceBB[8];
    memcpy(newPieceBB, board.pieceBB, 8 * sizeof(U64));
    U16 newGameInfo = board.gameInfo;

    Board::applyMove(move, newPieceBB, newGameInfo, board);

    return Board(newPieceBB, newGameInfo);
}

Board Game::makeMove(Board& fromBoard, const Move& move) {
    U64 newPieceBB[8];
    memcpy(newPieceBB, fromBoard.pieceBB, 8 * sizeof(U64));
    U16 newGameInfo = fromBoard.gameInfo;

    Board::applyMove(move, newPieceBB, newGameInfo, fromBoard);

    return Board(newPieceBB, newGameInfo);
}

GameState Game::returnGameState(Board& board){
    MovesStruct legalMoves = moveGen.generateAllLegalMoves(board);
    GameState tempState = ONGOING;

    if (legalMoves.count == 0) {
        if (board.isInCheck()) {
            tempState = CHECKMATE;
        } else {
            tempState = STALEMATE;
        }
    } else if (isThreefoldRepetition()) {
            tempState = DRAW_INSUFFICIENT_MATERIAL;
    } else if (isFiftyMoveRule()) {
            tempState = DRAW_50_MOVE;
    } else if (isInsufficientMaterial()) {
            tempState = DRAW_INSUFFICIENT_MATERIAL;
    } 

    return tempState;
}

void Game::updateGameState() {
    state = returnGameState(board);
}

