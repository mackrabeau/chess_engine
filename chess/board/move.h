#ifndef MOVE_H
#define MOVE_H

#include "types.h"

#include <cstdint>
#include <iostream>

typedef uint16_t U16;
using namespace std;

class Move {
public:

    Move() : move(0) {}
    Move(U16 m) : move(m) {}

    U16 getMove() const { return move; }
    
    int getFrom() const { return (move >> 6) & 0x3F; } // extract from square
    int getTo() const { return move & 0x3F; }
    int getFlags() const { return move >> 12; } // extract flags

    void setFrom(int from) { move = (move & 0x3F) | (from << 6); } // set from square
    void setTo(int to) { move = (move & 0xFC0) | (to & 0x3F); }
    void setFlags(int flags) { move = (move & 0xFFF) | (flags << 12); } // set flags

    bool isQuiet() const { return (move >> 12) == QUIET_MOVES; } // quiet move
    bool isDoublePawnPush() const { return (move >> 12) == DOUBLE_PAWN_PUSH; }
    bool isKingCastle() const { return (move >> 12) == KING_CASTLE; }
    bool isQueenCastle() const { return (move >> 12) == QUEEN_CASTLE; }
    bool isCapture() const { return (((move >> 12) == CAPTURE) || (move >> 12) == QUEEN_PROMO_CAPTURE || (move >> 12) == EP_CAPTURE); }
    bool isEPCapture() const { return (move >> 12) == EP_CAPTURE; }
    bool isPromotion() const { return (move >> 12) >= KNIGHT_PROMO && (move >> 12) <= QUEEN_PROMO; } // knight, bishop, rook, queen promotion
    bool isPromoCapture() const { return (move >> 12) >= KNIGHT_PROMO_CAPTURE && (move >> 12) <= QUEEN_PROMO_CAPTURE; } // knight, bishop, rook, queen promo capture
    int getPromotionType() const { return (move >> 13) & 0x3; }
    
    void setPromotionType(int type){ 
        move &= ~(0x3 << 13); 
        move |= (type << 13);  
    }// clear bits 13 and 14, set to new type


    void display() const {
        std::cout << "Move: " << move << ", From: " << getFrom() << ", To: " << getTo() << ", Flags: " << getFlags() << "\n";
    }



    std::string toString() const {
        std::string moveStr;
        moveStr += 'a' + (getFrom() % 8); // file
        moveStr += '1' + (getFrom() / 8); // rank
        moveStr += 'a' + (getTo() % 8); // file
        moveStr += '1' + (getTo() / 8); // rank
    
        return moveStr;
    }

private:
    U16 move;
};

static const int MAX_MOVES = 218; // max number of moves

struct MovesStruct {
    Move moveList[MAX_MOVES]; // max number of moves
    short count; // counts number of moves

    MovesStruct() : count(0) {}

    void addMove(Move move){
        if (count < MAX_MOVES) {
            moveList[count] = move;
            count++;
        } else {
            std::cout << "Move list is full, cannot add more moves. fix later\n";
        }
    }

    void clear() {
        count = 0;
    }

    void displayMoves() const {
        for (int i = 0; i < count; i++) {
            displayMove(i);
        }
    }

    void displayMove(int i) const {
        if (i < 0 || i >= count) return; // invalid index

        moveList[i].display();
    }

    int getNumMoves() const { return count; } // returns number of legal moves 
    Move getMove(int i) const { return moveList[i].getMove(); }

};


#endif // MOVE_H

