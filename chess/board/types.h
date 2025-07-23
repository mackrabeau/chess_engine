#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

typedef uint64_t U64;
typedef uint16_t U16;
typedef uint8_t U8;

enum enumPiece {
    nBlack = 0,     // all black pieces
    nWhite = 1,     // all white pieces
    nPawns = 2,
    nKnights = 3,
    nBishops = 4,
    nRooks = 5,
    nQueens = 6,
    nKings = 7,
    nEmpty = 8    
};

enum moveType {
    QUIET_MOVES = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EP_CAPTURE = 5,
    KNIGHT_PROMO = 8,
    BISHOP_PROMO = 9,
    ROOK_PROMO = 10,
    QUEEN_PROMO = 11,
    KNIGHT_PROMO_CAPTURE = 12,
    BISHOP_PROMO_CAPTURE = 13,
    ROOK_PROMO_CAPTURE = 14,
    QUEEN_PROMO_CAPTURE = 15
};
// code	promotion	capture	special 1	special 0	kind of move
// 0	0	0	0	0	quiet moves
// 1	0	0	0	1	double pawn push
// 2	0	0	1	0	king castle
// 3	0	0	1	1	queen castle
// 4	0	1	0	0	captures
// 5	0	1	0	1	ep-capture
// 8	1	0	0	0	knight-promotion
// 9	1	0	0	1	bishop-promotion
// 10	1	0	1	0	rook-promotion
// 11	1	0	1	1	queen-promotion
// 12	1	1	0	0	knight-promo capture
// 13	1	1	0	1	bishop-promo capture
// 14	1	1	1	0	rook-promo capture
// 15	1	1	1	1	queen-promo capture


// Move flag bits (bits 12–15)
constexpr U16 FLAG_SPECIAL_0     = (1 << 12); // bit 12
constexpr U16 FLAG_SPECIAL_1     = (1 << 13); // bit 13
constexpr U16 FLAG_CAPTURE       = (1 << 14); // bit 14
constexpr U16 FLAG_PROMOTION     = (1 << 15); // bit 15

// Specific move types (for clarity)
constexpr U16 FLAG_DOUBLE_PAWN_PUSH = FLAG_SPECIAL_0;                  // 0001
constexpr U16 FLAG_KING_CASTLE      = FLAG_SPECIAL_1;                  // 0010
constexpr U16 FLAG_QUEEN_CASTLE     = FLAG_SPECIAL_1 | FLAG_SPECIAL_0; // 0011
constexpr U16 FLAG_CAPTURE_MOVE     = FLAG_CAPTURE;                    // 0100
constexpr U16 FLAG_EP_CAPTURE       = FLAG_CAPTURE | FLAG_SPECIAL_0;   // 0101

// Promotion types (promotion flag + special bits)
constexpr U16 FLAG_PROMO_KNIGHT = FLAG_PROMOTION;                      // 1000
constexpr U16 FLAG_PROMO_BISHOP = FLAG_PROMOTION | FLAG_SPECIAL_0;     // 1001
constexpr U16 FLAG_PROMO_ROOK   = FLAG_PROMOTION | FLAG_SPECIAL_1;     // 1010
constexpr U16 FLAG_PROMO_QUEEN  = FLAG_PROMOTION | FLAG_SPECIAL_1 | FLAG_SPECIAL_0; // 1011

// Bit masks for gameInfo
constexpr U16 TURN_MASK      = 0x1;      // bit 0
constexpr U16 WK_CASTLE      = 0x2;      // bit 1 (white-king castle)
constexpr U16 WQ_CASTLE      = 0x4;      // bit 2 (white-queen castle)
constexpr U16 BK_CASTLE      = 0x8;      // bit 3 (black-king castle)
constexpr U16 BQ_CASTLE      = 0x10;     // bit 4 (black-queen castle)

constexpr U16 MOVE_MASK      = 0x7E0;    // bits 5-10 (6 bits for move count)
constexpr int MOVE_SHIFT     = 5;

constexpr U16 EP_IS_SET   = (1 << 11);      // bit 11
constexpr U16 EP_FILE_MASK = (0x7 << 12);   // bits 12-14 (3 bits for file)
constexpr int EP_FILE_SHIFT = 12;

// White king-side castle (e1 to g1): squares f1 (5), g1 (6)
constexpr U64 WK_CASTLE_MASK  = (1ULL << 5) | (1ULL << 6);
// White queen-side castle (e1 to c1): squares d1 (3), c1 (2), b1 (1)
constexpr U64 WQ_CASTLE_MASK = (1ULL << 3) | (1ULL << 2) | (1ULL << 1);

// Black king-side castle (e8 to g8): squares f8 (61), g8 (62)
constexpr U64 BK_CASTLE_MASK  = (1ULL << 61) | (1ULL << 62);
// Black queen-side castle (e8 to c8): squares d8 (59), c8 (58), b8 (57)
constexpr U64 BQ_CASTLE_MASK = (1ULL << 59) | (1ULL << 58) | (1ULL << 57);


#endif // TYPES_H