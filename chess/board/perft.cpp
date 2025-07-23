#include "board.h"
#include "movegenerator.h"
#include "game.h"
#include "move.h"


#include <iostream>
#include <cassert>

using namespace std;

struct PerftStats {
    U64 nodes = 0;
    U64 captures = 0;
    U64 ep = 0;
    U64 castles = 0;
    U64 promotions = 0;
    U64 checks = 0;
    U64 discoveryChecks = 0;
    U64 doubleChecks = 0;
    U64 checkmates = 0;
};

int perft(Game& game, Board& board, int depth, int moveTypeTable[16]) {
    if (depth == 0) return 1;
    
    U64 nodes = 0;
    MovesStruct  legalMoves = game.moveGen.generateAllLegalMoves(board);


    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Board nextBoard = game.makeMove(board, legalMoves.getMove(i));
        nodes += perft(game, nextBoard, depth - 1, moveTypeTable);
    }

    return nodes;
}

void perft(Game& game, Board& board, int depth, PerftStats& stats, int maxBoardsDisplayed) {
    if (depth == 0) {
        stats.nodes++;
        return;
    }

    MovesStruct legalMoves = game.moveGen.generateAllLegalMoves(board);

    if (legalMoves.getNumMoves() == 0 ) {
        if (board.isInCheck()) {
            stats.checkmates++;
        }
        return;
    }

    for (int i = 0; i < legalMoves.getNumMoves(); ++i) {
        Move move = legalMoves.getMove(i);
        Board nextBoard = game.makeMove(board, move);

        if (depth == 1){

            if (move.isCapture() && !move.isEPCapture()) stats.captures++;
            if (move.isKingCastle() || move.isQueenCastle()) stats.castles++;
            if (move.isPromotion() || move.isPromoCapture()) stats.promotions++;
            if (move.isEPCapture()) stats.ep++; 

            if (nextBoard.isInCheck()) {
                stats.checks++;
                // For double/discovery checks, you need to implement logic to count attackers
                // stats.doubleChecks++;
                // stats.discoveryChecks++;
            }

            // if (move.isEPCapture() && maxBoardsDisplayed > 0) {
            //     // nextBoard.displayBoard();
            //     maxBoardsDisplayed--;
            // }
        }
        
        perft(game, nextBoard, depth - 1, stats, maxBoardsDisplayed);
    }
}

int main() {
    // Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Game game("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ");
    Game game("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    Game game2("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    int maxDepth = 5;

    std::cout << "Depth\tNodes\tCaptures\tE.p.\tCastles\tPromotions\tChecks\tDiscovery Checks\tDouble Checks\tCheckmates\n";
    for (int d = 1; d <= maxDepth; ++d) {
        PerftStats stats;
        perft(game, game.board, d, stats, 5);
        std::cout << d << "\t"
                  << stats.nodes << "\t"
                  << stats.captures << "\t"
                  << stats.ep << "\t"
                  << stats.castles << "\t"
                  << stats.promotions << "\t"
                  << stats.checks << "\t"
                  << stats.discoveryChecks << "\t"
                  << stats.doubleChecks << "\t"
                  << stats.checkmates << "\n";
    }
    std::cout << "Depth\tNodes\tCaptures\tE.p.\tCastles\tPromotions\tChecks\tDiscovery Checks\tDouble Checks\tCheckmates\n";
    for (int d = 1; d <= maxDepth; ++d) {
        PerftStats stats;
        perft(game2, game2.board, d, stats, 5);
        std::cout << d << "\t"
                  << stats.nodes << "\t"
                  << stats.captures << "\t"
                  << stats.ep << "\t"
                  << stats.castles << "\t"
                  << stats.promotions << "\t"
                  << stats.checks << "\t"
                  << stats.discoveryChecks << "\t"
                  << stats.doubleChecks << "\t"
                  << stats.checkmates << "\n";
    }


    // Game game2("rnbqkbnr/ppp2ppp/8/1B1pp3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 1 3");
    // // Game game2("rnb1kbnr/pppp1ppp/8/4p3/4PP1q/8/PPPP2PP/RNBQKBNR w KQkq - 1 3");


    // MovesStruct moves = game2.moveGen.generateAllLegalMoves(game2.board);
    // game2.board.displayBoard();
    
    // cout << "==================\n";

    // for (int i = 0; i < moves.getNumMoves() && i < 10; ++i) {
    //     Move move = moves.getMove(i);

    //     Board nextBoard = game2.makeMove(game2.board, move);
    //     nextBoard.displayBoard();

    // }
    // cout << moves.getNumMoves() << " legal moves generated.\n";


    return 0;
}
