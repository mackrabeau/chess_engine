#include "board.h"
#include "game.h"
#include "move.h"

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

// Helper function to print detailed board state
void printBoardState(const Game& game, const string& label) {
    cout << "\n=== " << label << " ===" << endl;
    cout << "FEN: " << game.board.toString() << endl;
    cout << "Current Player: " << (game.board.friendlyColour() == nWhite ? "White" : "Black") << endl;
    cout << "History Size: " << game.historySize << endl;
    cout << "Game State: " << game.state << endl;
    cout << "Hash: 0x" << hex << game.board.hash << dec << endl;
    
    // Print piece counts
    int whitePieces = __builtin_popcountll(game.board.pieceBB[nWhite]);
    int blackPieces = __builtin_popcountll(game.board.pieceBB[nBlack]);
    cout << "White pieces: " << whitePieces << ", Black pieces: " << blackPieces << endl;
    cout << "---" << endl;
}

// Helper to print move details
void printMoveDetails(const Move& move, const string& label) {
    cout << "\n" << label << ":" << endl;
    cout << "  From: " << move.getFrom() << " (" << (char)('a' + move.getFrom() % 8) << (move.getFrom() / 8 + 1) << ")" << endl;
    cout << "  To: " << move.getTo() << " (" << (char)('a' + move.getTo() % 8) << (move.getTo() / 8 + 1) << ")" << endl;
    cout << "  Flags: " << move.getFlags() << endl;
    cout << "  Captured: " << move.getCapturedPiece() << endl;
}

void testBasicPawnMove() {
    cout << "\n🧪 Testing Basic Pawn Move (e2-e4)" << endl;

    Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    printBoardState(game, "Initial Position");

    MovesStruct moves = game.generateAllLegalMoves();
    Move move = moves.getMove(0);

    printMoveDetails(move, "Move to test");
    game.board.displayBoard();

    cout << "\n📤 Pushing move..." << endl;
    game.pushMove(move);

    printBoardState(game, "After Push");
    game.board.displayBoard();

    game.popMove();


    printBoardState(game, "After Pop");
    game.board.displayBoard();



}

int main() {

    testBasicPawnMove();

}