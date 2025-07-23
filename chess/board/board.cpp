#include "board.h"
#include <cstring>
#include <sstream>

using namespace std;

Board::Board(const std::string& fen){
    clearBoard();
    gameInfo = 0ULL;

    std::istringstream iss(fen);
    std::string boardPart, turnPart, castlingPart, epPart, halfmovePart, fullmovePart;

    // Parse the FEN string
    iss >> boardPart >> turnPart >> castlingPart >> epPart >> halfmovePart >> fullmovePart;

    // 1. Piece placement
    int square = 56; // Start at a8
    for (char c : boardPart) {
        if (c == '/') {
            square -= 16; // Move to next rank
        } else if (isdigit(c)) {
            square += c - '0';
        } else {
            loadPiece(c, square);
            square++;
        }
    }

    // 2. Turn
    if (turnPart == "w") {
        gameInfo |= TURN_MASK; // White to move
    } else if (turnPart == "b") {
        gameInfo &= ~TURN_MASK; // Black to move
    }

    // 3. Castling rights
    for (char c : castlingPart) {
        switch (c) {
            case 'K': gameInfo |= WK_CASTLE; break;
            case 'Q': gameInfo |= WQ_CASTLE; break;
            case 'k': gameInfo |= BK_CASTLE; break;
            case 'q': gameInfo |= BQ_CASTLE; break;
            case '-': break;
        }
    }
    // 4. Move count (halfmove clock)
    if (!halfmovePart.empty()) {
        int moveCount = std::stoi(halfmovePart);
        gameInfo &= ~MOVE_MASK;
        gameInfo |= (moveCount << 6) & MOVE_MASK;
    }
    
    // 5. En passant square
    if (epPart != "-" && !epPart.empty()) {
        char fileChar = epPart[0];
        int file = fileChar - 'a';
        // Store file in en passant bits
        gameInfo &= ~(EP_IS_SET | EP_FILE_MASK);
        gameInfo |= EP_IS_SET | ((file << EP_FILE_SHIFT) & EP_FILE_MASK);
    } else {
        // No en passant
        gameInfo &= ~(EP_IS_SET | EP_FILE_MASK); // Clear old ep info
    }

    //     // 6. Fullmove number (optional, usually not needed for move generation)
    // if (!fullmovePart.empty()) {
    //     int fullmoveNumber = std::stoi(fullmovePart);
    //     // You can store this in a member variable if you want, but it's not needed for move legality.
    // }

};

Board::Board(const Board& other){
    for (int i = 0; i < 8; i++){
        pieceBB[i] = other.pieceBB[i];
    }
    gameInfo = other.gameInfo;
}

Board::Board(U64 otherPieceBB[8], const U16& otherGameInfo){
    memcpy(pieceBB, otherPieceBB, 8 * sizeof(U64));
    gameInfo = otherGameInfo;
}

std::string Board::toString() const {
    std::string fen;
    
    // 1. Piece placement (from rank 8 to 1)
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char piece = pieceToChar(square);
            
            if (piece == '.') {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += piece;
            }
        }
        
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }
        
        if (rank > 0) {
            fen += '/';
        }
    }
    
    // 2. Active color
    fen += ' ';
    fen += (gameInfo & TURN_MASK) ? 'w' : 'b';
    
    // 3. Castling availability
    fen += ' ';
    bool hasCastlingRights = false;
    if (gameInfo & WK_CASTLE) { fen += 'K'; hasCastlingRights = true; }
    if (gameInfo & WQ_CASTLE) { fen += 'Q'; hasCastlingRights = true; }
    if (gameInfo & BK_CASTLE) { fen += 'k'; hasCastlingRights = true; }
    if (gameInfo & BQ_CASTLE) { fen += 'q'; hasCastlingRights = true; }
    if (!hasCastlingRights) { fen += '-'; }
    
    // 4. En passant target square
    fen += ' ';
    if (gameInfo & EP_IS_SET) {
        int epFile = (gameInfo & EP_FILE_MASK) >> EP_FILE_SHIFT;
        int epRank = (gameInfo & TURN_MASK) ? 6 : 3; // If white to move, ep square is on rank 6; if black, rank 3
        
        fen += static_cast<char>('a' + epFile);
        fen += static_cast<char>('0' + epRank);
    } else {
        fen += '-';
    }
    
    // 5. Halfmove clock (for 50-move rule)
    int halfmoveClock = (gameInfo & MOVE_MASK) >> 6; // Using 6 as the shift based on code context
    fen += ' ' + std::to_string(halfmoveClock);
    
    // 6. Fullmove number +++ IMPLEMENT LATER
    // fen += "";
    return fen;
}
int Board::getEnPassantSquare() const {
    if (!(gameInfo & EP_IS_SET)) return -1; // No en passant square

    int epCol = (gameInfo & EP_FILE_MASK) >> EP_FILE_SHIFT; // get en passant column from game info
    int epRow = (friendlyColour() == nWhite) ? 5 : 2; // row 5 for white, 2 for black
    return (epRow * 8 + epCol);
}

void Board::applyMove(const Move& move, U64* pieceBBTarget, U16& gameInfoTarget, const Board& board) {

    U8 from = move.getFrom(); // extract from square, mask to 6 bits
    U8 to = move.getTo(); // extract to square, mask to 6 bits
    U16 moveInt = move.getMove(); // get the full move

    enumPiece piece = board.getPieceType(from);
    enumPiece colour = board.getColourType(from);

    // remove the piece from the "from" square
    pieceBBTarget[piece] &= ~(1ULL << from); 
    pieceBBTarget[colour] &= ~(1ULL << from);

    // clear en passant square if not a double pawn push,
    // if the move is a double pawn push, set the en passant square later
    gameInfoTarget &= ~(EP_IS_SET | EP_FILE_MASK);

    // capture logic
    if (move.isCapture()) {
        if (move.isEPCapture()) {
            // determine the square behind the captured pawn and remove pawn from the board
            int enPassantSquare = (colour == nWhite) ? to - 8 : to + 8; 
            pieceBBTarget[nPawns] &= ~(1ULL << enPassantSquare);
            pieceBBTarget[(colour == nWhite ? nBlack : nWhite)] &= ~(1ULL << enPassantSquare);

        } else {
            // for all other captures, remove piece
            enumPiece capturedPiece = board.getPieceType(to);
            enumPiece capturedColour = board.getColourType(to);

            pieceBBTarget[capturedPiece] &= ~(1ULL << to);
            pieceBBTarget[capturedColour] &= ~(1ULL << to);
        }
    }

    if (move.isPromoCapture() || move.isPromotion()) {
        // handle promotion
        enumPiece promotedPiece = nQueens; // default to queen promotion
        pieceBBTarget[promotedPiece] |= (1ULL << to);

    } else {
        // normal move, just update the target square
        pieceBBTarget[piece] |= (1ULL << to);

        if (piece == nPawns) {
            if (abs((int)from - (int)to) == 16) {
                // double pawn push
                int file = to % 8; // get the file of the pawn
                // set the en passant square
                gameInfoTarget |= EP_IS_SET | ((file << EP_FILE_SHIFT) & EP_FILE_MASK);
            }
        } 
    }

    // add piece to the "to" square
    pieceBBTarget[colour] |= (1ULL << to);

    if (move.isCapture() || piece == nPawns){
        gameInfoTarget &= ~MOVE_MASK; // reset halfmove clock
    } else {
        gameInfoTarget = (gameInfoTarget & ~MOVE_MASK) | (((((gameInfoTarget & MOVE_MASK) >> 6) + 1) << 6) & MOVE_MASK);
    }

    // castling rights
    if (piece == nKings) {
        // remove castling rights for the king
        if (colour == nWhite) {
            gameInfoTarget &= ~(WK_CASTLE | WQ_CASTLE);
        } else {
            gameInfoTarget &= ~(BK_CASTLE | BQ_CASTLE);
        }
    } else if (piece == nRooks) {
        // remove castling rights for the rook
        if (colour == nWhite) {
            if (from == 0) gameInfoTarget &= ~WQ_CASTLE; // a1 rook
            if (from == 7) gameInfoTarget &= ~WK_CASTLE; // h1 rook
        } else {
            if (from == 56) gameInfoTarget &= ~BQ_CASTLE; // a8 rook
            if (from == 63) gameInfoTarget &= ~BK_CASTLE; // h8 rook
        }
    }
    // if castling, update the rook's position
    if (move.isKingCastle() || move.isQueenCastle()) {
        if (colour == nWhite) {
            if (to == 2) { // queenside castle
                pieceBBTarget[nRooks] &= ~(1ULL << 0); // remove a1 rook
                pieceBBTarget[nRooks] |= (1ULL << 3); // add d1 rook
                pieceBBTarget[nWhite] &= ~(1ULL << 0);
                pieceBBTarget[nWhite] |= (1ULL << 3);
            } else if (to == 6) { // kingside castle
                pieceBBTarget[nRooks] &= ~(1ULL << 7); // remove h1 rook
                pieceBBTarget[nRooks] |= (1ULL << 5); //   add f1 rook     
                pieceBBTarget[nWhite] &= ~(1ULL << 7);
                pieceBBTarget[nWhite] |= (1ULL << 5);
            }
        } else {
            if (to == 58) { // queenside castle
                pieceBBTarget[nRooks] &= ~(1ULL << 56); // remove a8 rook
                pieceBBTarget[nRooks] |= (1ULL << 59); // add d8 rook
                pieceBBTarget[nBlack] &= ~(1ULL << 56);
                pieceBBTarget[nBlack] |= (1ULL << 59);     
            } else if (to == 62) { // kingside castle
                pieceBBTarget[nRooks] &= ~(1ULL << 63); // remove h8 rook
                pieceBBTarget[nRooks] |= (1ULL << 61); // add f8 rook
                pieceBBTarget[nBlack] &= ~(1ULL << 63);
                pieceBBTarget[nBlack] |= (1ULL << 61);
            }
        }
    }   

    gameInfoTarget ^= TURN_MASK;  // switch turns
}


bool Board::isInCheck() const {
    return isInCheck(friendlyColour());
}

bool Board::isInCheck(U8 colour) const {
    // Find the king's bitboard for the given colour
    U64 kingBB = 0ULL;
    U8 enemyColour;

    if (colour == nWhite) {
        kingBB = getWhiteKing();
        enemyColour = nBlack;
    } else {
        kingBB = getBlackKing();
        enemyColour = nWhite;
    }

    // If there is no king on the board, not in check (defensive)
    if (kingBB == 0) return false;

    // Use MoveGenerator to get all squares attacked by the enemy
    MoveGenerator moveGen(MoveTables::instance());
    U64 attacked = moveGen.attackedBB(*this, enemyColour);

    // If the king's square is attacked, return true
    return (kingBB & attacked) != 0;
}

void Board::clearBoard(){
    for (int i = 0; i < 8; i++) {
        pieceBB[i] = 0;  // clear all the bitboards
    }
}

void Board::displayBoard() const {
    for (int r = 7; r >= 0; r --){
        for (int c = 0; c < 8; c ++){

            int square = r * 8 + c;
            std::cout<< pieceToChar(square) << " ";
        }
        std::cout<< "\n";
    }
    // coloursTurnToString();
    displayGameInfo();
    std::cout << "------------------------\n";
}

void Board::displayGameInfo() const {
    // Turn
    std::cout << "Turn: " << ((gameInfo & TURN_MASK) ? "White" : "Black") << std::endl;

    // Castling rights
    std::cout << "Castling rights: ";
    bool any = false;
    if (gameInfo & WK_CASTLE) { std::cout << "K"; any = true; }
    if (gameInfo & WQ_CASTLE) { std::cout << "Q"; any = true; }
    if (gameInfo & BK_CASTLE) { std::cout << "k"; any = true; }
    if (gameInfo & BQ_CASTLE) { std::cout << "q"; any = true; }
    if (!any) std::cout << "-";
    std::cout << std::endl;

    // Halfmove clock
    int halfmoveClock = (gameInfo & MOVE_MASK) >> MOVE_SHIFT;
    std::cout << "Halfmove clock: " << halfmoveClock << std::endl;

    // En passant square
    int epFile = (gameInfo & EP_FILE_MASK) >> EP_FILE_SHIFT;
    if ((gameInfo & EP_IS_SET) == 0) {
        std::cout << "En passant: -" << std::endl;
    } else {
        char fileChar = 'a' + epFile;
        int rank = (gameInfo & TURN_MASK) ? 6 : 3; // If white to move, ep is on rank 6; if black, rank 3
        std::cout << "En passant: " << fileChar << rank << std::endl;
    }
}

void Board::coloursTurnToString() const{
    // Display the current turn (1 for white, 0 for black)
    if (gameInfo & TURN_MASK) {
        std::cout << "White's turn" << std::endl;
    } else {
        std::cout << "Black's turn" << std::endl;
    }
}

void Board::loadPiece(char piece, int square) {
    
    if (piece == toupper(piece)) {
        pieceBB[nWhite] = pieceBB[nWhite] | (1ULL << square);
    } else {
        pieceBB[nBlack] = pieceBB[nBlack] | (1ULL << square);
    }

    piece = toupper(piece);
    
    if (piece == 'P') {pieceBB[nPawns] = pieceBB[nPawns] | (1ULL << square); return;}
    if (piece == 'N') {pieceBB[nKnights] = pieceBB[nKnights] | (1ULL << square); return;}
    if (piece == 'B') {pieceBB[nBishops] = pieceBB[nBishops] | (1ULL << square); return;}
    if (piece == 'R') {pieceBB[nRooks] = pieceBB[nRooks] | (1ULL << square); return;}
    if (piece == 'Q') {pieceBB[nQueens] = pieceBB[nQueens] | (1ULL << square); return;}
    if (piece == 'K') {pieceBB[nKings] = pieceBB[nKings] | (1ULL << square); return;}
}

enumPiece Board::getPieceType(int square) const {
    if (pieceBB[nPawns] >> square & 1) return nPawns;
    if (pieceBB[nBishops] >> square & 1) return nBishops;
    if (pieceBB[nKnights] >> square & 1) return nKnights;
    if (pieceBB[nRooks] >> square & 1) return nRooks;
    if (pieceBB[nQueens] >> square & 1) return nQueens;
    if (pieceBB[nKings] >> square & 1) return nKings;
    return nEmpty;
}

enumPiece Board::getColourType(int square) const {
    if (pieceBB[nWhite] >> square & 1){return nWhite;}
    if (pieceBB[nBlack] >> square & 1){return nBlack;}
    // return nEmpty;
    throw std::invalid_argument( "received empty square");
}

char Board::pieceToChar(int square) const {
    if (getWhitePawns() >> square & 1){return 'P';}
    if (getBlackPawns() >> square & 1){return 'p';}

    if (getWhiteBishops() >> square & 1){return 'B';}
    if (getBlackBishops() >> square & 1){return 'b';}

    if (getWhiteKnights() >> square & 1){return 'N';}
    if (getBlackKnights() >> square & 1){return 'n';}

    if (getWhiteRooks() >> square & 1){return 'R';}
    if (getBlackRooks() >> square & 1){return 'r';}

    if (getWhiteQueens() >> square & 1){return 'Q';}
    if (getBlackQueens() >> square & 1){return 'q';}

    if (getWhiteKing() >> square & 1){return 'K';}
    if (getBlackKing() >> square & 1){return 'k';}
    return '.';
}

int Board::colourCode(enumPiece ct) const {
    return static_cast<int>(ct);
}

int Board::pieceCode(enumPiece pt) const {
    return static_cast<int>(pt);
}




