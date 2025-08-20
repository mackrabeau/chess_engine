#include "evaluation.h"

// simplified evaluation function

int evaluateBoard(const Board& board) {

    U64 pieces = board.getAllPieces();
    
    int score = 0;

    while (pieces) {
        int square = __builtin_ctzll(pieces);  // get the least significant set bit
        pieces &= pieces - 1;                  // remove the least significant set bit

        enumPiece pieceType = board.getPieceType(square);
        enumPiece pieceColour = board.getColourType(square);

        switch (pieceType) {
            case nPawns:
                pieceColour == nWhite ? score += wpTable[square] : score -= bpTable[square]; // Add pawn evaluation logic
                break;
            case nKnights:
                pieceColour == nWhite ? score += wnTable[square] : score -= bnTable[square]; // Add knight evaluation logic
                break;
            case nBishops:
                pieceColour == nWhite ? score += wbTable[square] : score -= bbTable[square]; // Add bishop evaluation logic
                break;
            case nRooks:
                pieceColour == nWhite ? score += wrTable[square] : score -= brTable[square]; // Add rook evaluation logic
                break;
            case nQueens:
                pieceColour == nWhite ? score += wqTable[square] : score -= bqTable[square]; // Add queen evaluation logic
                break;
            case nKings:
                pieceColour == nWhite ? score += wkMidTable[square] : score -= bkMidTable[square]; // Add king evaluation logic
                break;
            default:
                break;
        }
    }

    return score + materialScore(board);

    // int v = score + materialScore(board);
    // return (board.gameInfo & 1) ? v : -v; // if black to move, invert scores
}

// dont use for now
int materialScore(const Board& board) {
    
    int score = 0;

    score += __builtin_popcountll(board.getWhitePawns()) * Pwt;   // Pawns
    score += __builtin_popcountll(board.getWhiteKnights()) * Nwt; // Knights
    score += __builtin_popcountll(board.getWhiteBishops()) * Bwt; // Bishops
    score += __builtin_popcountll(board.getWhiteRooks()) * Rwt;   // Rooks
    score += __builtin_popcountll(board.getWhiteQueens()) * Qwt;  // Queens

    score -= __builtin_popcountll(board.getBlackPawns()) * Pwt;
    score -= __builtin_popcountll(board.getBlackKnights()) * Nwt;
    score -= __builtin_popcountll(board.getBlackBishops()) * Bwt;
    score -= __builtin_popcountll(board.getBlackRooks()) * Rwt;
    score -= __builtin_popcountll(board.getBlackQueens()) * Qwt;

    return score;
}

