#include "evaluation.h"

// simplified evaluation function

int evaluateBoard(const Board& board) {

    U64 pieces = board.getAllPieces();
    
    int score = 0;

    while (pieces) {
        int square = __builtin_ctzll(pieces);  // get the least significant set bit
        pieces &= pieces - 1;                  // remove the least significant set bit

        enumPiece pieceType = board.getPieceType(square);
        switch (pieceType) {
            case nPawns:
                board.getWhitePieces() & (1ULL << square) ? score += (Pwt + pawnTable[square]) : score -= (Pwt + pawnTable[square]); // Add pawn evaluation logic
                break;
            case nKnights:
                board.getWhitePieces() & (1ULL << square) ? score += (Nwt + knightTable[square]) : score -= (Nwt + knightTable[square]); // Add knight evaluation logic
                break;
            case nBishops:
                board.getWhitePieces() & (1ULL << square) ? score += (Bwt + bishopTable[square]) : score -= (Bwt + bishopTable[square]); // Add bishop evaluation logic
                break;
            case nRooks:
                board.getWhitePieces() & (1ULL << square) ? score += (Rwt + rookTable[square]) : score -= (Rwt + rookTable[square]); // Add rook evaluation logic
                break;
            case nQueens:
                board.getWhitePieces() & (1ULL << square) ? score += (Qwt + queenTable[square]) : score -= (Qwt + queenTable[square]); // Add queen evaluation logic
                break;
            case nKings:
                // temperary logic for king evaluation
                // In a real evaluation, you would want to consider king safety, control of the center
                board.getWhitePieces() & (1ULL << square) ? score += Kwt : score -= Kwt; // King is usually not counted in material, but can be evaluated for safety
                score += 0; // King table evaluation can be added later
                break;
            default:
                break;
        }
    }

    return score;
}

// dont use for now
int material_score(const Board& board) {
    int score = 0;

    // Add material values for each piece type
    score += __builtin_popcountll(board.getWhitePawns()) * Pwt;   // Pawns
    score += __builtin_popcountll(board.getWhiteKnights()) * Nwt; // Knights
    score += __builtin_popcountll(board.getWhiteBishops()) * Bwt; // Bishops
    score += __builtin_popcountll(board.getWhiteRooks()) * Rwt;   // Rooks
    score += __builtin_popcountll(board.getWhiteQueens()) * Qwt;  // Queens
    score += __builtin_popcountll(board.getWhiteKing()) * Kwt;    // King (not usually counted, but for completeness)

    score -= __builtin_popcountll(board.getBlackPawns()) * Pwt;
    score -= __builtin_popcountll(board.getBlackKnights()) * Nwt;
    score -= __builtin_popcountll(board.getBlackBishops()) * Bwt;
    score -= __builtin_popcountll(board.getBlackRooks()) * Rwt;
    score -= __builtin_popcountll(board.getBlackQueens()) * Qwt;
    score -= __builtin_popcountll(board.getBlackKing()) * Kwt;

    return score;
}

