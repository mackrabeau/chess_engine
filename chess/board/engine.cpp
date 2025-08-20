#include "board.h"
#include "game.h"
#include "move.h"
#include "evaluation.h"
#include "transposition.h"
#include "search.h"

#include <iostream>
#include <sstream>
#include <chrono>

using namespace std;

std::chrono::steady_clock::time_point g_searchStartTime;
long g_timeLimit = 20000;  // ms


std::string getBestMove(Game& game, int maxTimeMs = 2000) {

    game.enableFastMode();
    g_searchStartTime = std::chrono::steady_clock::now();
    g_timeLimit = maxTimeMs;
    resetSearchStats();

    Move bestMove;

    for (int depth = 1; depth <= MAX_SEARCH_DEPTH; ++depth) {
        
        string depthStr = "DEPTH:" + std::to_string(depth);
        std::cerr << depthStr << std::endl;

        Move depthBestMove = searchAtDepth(game, depth);

        if (isTimeUp()) break;

        if (depthBestMove.getMove()) {
            bestMove = depthBestMove;
        }
    }

    game.disableFastMode();
    printSearchStats();

    // no move found
    if (!(bestMove.getMove())) {
        GameState state = game.calculateGameState();
        std::string gameStateStr;
        switch (state) {
            case ONGOING: gameStateStr = "ongoing"; break;
            case CHECKMATE: gameStateStr = "checkmate"; break;
            case STALEMATE: gameStateStr = "stalemate"; break;
            case DRAW_REPETITION: gameStateStr = "draw_repetition"; break;
            case DRAW_50_MOVE: gameStateStr = "draw_50_move"; break;
            case DRAW_INSUFFICIENT_MATERIAL: gameStateStr = "draw_insufficient_material"; break;
            default: gameStateStr = "unknown"; break;
        }

        return gameStateStr;
    }

    return bestMove.toString();
}


int main() {
    MoveTables::instance().init(); // Initialize once
    Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // Initial state

    std::string line;
    while (std::getline(std::cin, line)) {

        std::istringstream iss(line);

        std::string requestId, command;
        iss >> requestId >> command;

        if (command == "quit") {
            break;

        } else if (command == "print") {
            game.board.displayBoard();

        } else if (command == "reset") {

            game.reset();
            g_transpositionTable.clear();  // Clear TT on reset
            std::cout << requestId << " ok" << std::endl;

        } else if (command == "search_tree") {
            if (g_recordSearchTree) {
                stopAndPrintSearchTree();
            } else {
                startSearchTree();
                std::cout << requestId << " search_tree_started" << std::endl;
            }


        } else if (command == "best") {
            std::string bestMove = getBestMove(game);

            std::cout << requestId << " " << bestMove << std::endl;

        } else if (command == "move") {
            std::string moveStr;
            iss >> moveStr;

            if (moveStr.length() < 4) {
                std::cout << requestId << " error: invalid move string" << std::endl;
                continue;
            }

            int fromSquare = (moveStr[0] - 'a') + (moveStr[1] - '1') * 8;
            int toSquare = (moveStr[2] - 'a') + (moveStr[3] - '1') * 8;

            if (!(game.isLegal(fromSquare, toSquare))) {
                std::cout << requestId << " error: illegal move" << std::endl;
                continue;
            }

            enumPiece promoPiece = nEmpty;
            if (moveStr.length() == 5) {
                char promoChar = moveStr[4];
                switch (promoChar) {
                    case 'n': promoPiece = nKnights; break;
                    case 'b': promoPiece = nBishops; break;
                    case 'r': promoPiece = nRooks;   break;
                    case 'q': promoPiece = nQueens;  break;
                    default:
                        std::cout << requestId << " error: invalid promotion piece" << std::endl;
                        continue;
                }
            } 

            Move move(fromSquare, toSquare, game.board.getEnPassantSquare(), game.board.getPieceType(fromSquare), game.board.getPieceType(toSquare), promoPiece);
            game.pushMove(move);

            std::cout << requestId << " " << game.board.toString() << std::endl;

        } else if (command == "state") {
            GameState state = game.calculateGameState();
            std::string gameStateStr;
            switch (state) {
                case ONGOING: gameStateStr = "ongoing"; break;
                case CHECKMATE: gameStateStr = "checkmate"; break;
                case STALEMATE: gameStateStr = "stalemate"; break;
                case DRAW_REPETITION: gameStateStr = "draw_repetition"; break;
                case DRAW_50_MOVE: gameStateStr = "draw_50_move"; break;
                case DRAW_INSUFFICIENT_MATERIAL: gameStateStr = "draw_insufficient_material"; break;
                default: gameStateStr = "unknown"; break;
            }

            std::cout << requestId << " " << gameStateStr << std::endl;

} else if (command == "debug_quiescence") {
    std::cerr << "=== QUIESCENCE SEARCH DEBUG ===" << std::endl;
    
    // Get current position evaluation
    int staticEval = evaluateBoard(game.board);
    std::cerr << "Static evaluation: " << staticEval << std::endl;
    
    // Run quiescence search
    int quiesceResult = quiescenceSearch(-30000, 30000, game, 0);
    std::cerr << "Quiescence result: " << quiesceResult << std::endl;
    std::cerr << "Difference: " << (quiesceResult - staticEval) << std::endl;
    
    // Analyze available captures
    MovesStruct captures = game.generateAllLegalMoves(true);
    std::cerr << "Available captures: " << captures.getNumMoves() << std::endl;
    
    for (int i = 0; i < captures.getNumMoves(); ++i) {
        Move move = captures.getMove(i);
        std::cerr << "Capture " << i << ": " << move.toString();
        
        if (move.getCapturedPiece() != nEmpty) {
            int victimValue = pieceScore(move.getCapturedPiece());
            int attackerValue = pieceScore(game.board.getPieceType(move.getFrom()));
            std::cerr << " (victim=" << victimValue << " attacker=" << attackerValue << ")";
            
            // Test the capture
            game.pushMove(move);
            int afterCapture = evaluateBoard(game.board);
            game.popMove();
            
            int gain = (game.board.gameInfo & 1) ? (afterCapture - staticEval) : (staticEval - afterCapture);
            std::cerr << " gain=" << gain;
        }
        std::cerr << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_capture_ordering") {
    std::cerr << "=== CAPTURE ORDERING DEBUG ===" << std::endl;
    
    MovesStruct captures = game.generateAllLegalMoves(true);
    std::vector<std::pair<Move, int>> scoredCaptures;
    
    for (int i = 0; i < captures.getNumMoves(); ++i) {
        Move move = captures.getMove(i);
        int moveScore = 0;
        
        if (move.getCapturedPiece() != nEmpty) {
            int victim = pieceScore(move.getCapturedPiece()) / 100;
            int attacker = pieceScore(game.board.getPieceType(move.getFrom())) / 100;
            moveScore = 1000 + (victim * 10) - attacker;
            
            std::cerr << "Move: " << move.toString() 
                     << " Victim: " << victim << " Attacker: " << attacker 
                     << " Score: " << moveScore << std::endl;
        }
        
        scoredCaptures.push_back({move, moveScore});
    }
    
    std::sort(scoredCaptures.begin(), scoredCaptures.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::cerr << "\nAfter sorting:" << std::endl;
    for (const auto& [move, score] : scoredCaptures) {
        std::cerr << "Move: " << move.toString() << " Score: " << score << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_move") {
    std::string moveStr;
    iss >> moveStr;
    if (!moveStr.empty()) {
        std::cerr << "=== DEBUG MOVE: " << moveStr << " ===" << std::endl;
        
        // Find and analyze this move
        MovesStruct moves = game.generateAllLegalMoves();
        for (int i = 0; i < moves.getNumMoves(); ++i) {
            Move move = moves.getMove(i);
            if (move.toString() == moveStr) {
                std::cerr << "Found move: " << move.toString() << std::endl;
                
                // Evaluate before move
                int beforeEval = evaluateBoard(game.board);
                std::cerr << "Eval before: " << beforeEval << std::endl;
                
                game.pushMove(move);
                
                // Evaluate after move
                int afterEval = evaluateBoard(game.board);
                std::cerr << "Eval after: " << afterEval << std::endl;
                std::cerr << "Change: " << (afterEval - beforeEval) << std::endl;
                
                // Check if in check
                if (game.isInCheck()) {
                    std::cerr << "❌ MOVE PUTS KING IN CHECK!" << std::endl;
                }
                
                // Check what pieces can be captured
                MovesStruct opponentMoves = game.generateAllLegalMoves();
                for (int j = 0; j < opponentMoves.getNumMoves(); ++j) {
                    Move oppMove = opponentMoves.getMove(j);
                    if (oppMove.getCapturedPiece() != nEmpty) {
                        int capturedValue = pieceScore(oppMove.getCapturedPiece());
                        std::cerr << "Opponent can capture: " << oppMove.toString() 
                                 << " (value: " << capturedValue << ")" << std::endl;
                    }
                }
                
                game.popMove();
                break;
            }
        }
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

            
        } else if (command == "eval") {

            
            int evaluation = evaluateBoard(game.board);
            std::cout << requestId << " " << evaluation << std::endl;

        } else if (command == "position") {
            // Return current board position as FEN
            std::cout << requestId << " " << game.board.toString() << std::endl;

        } else if (command == "tt_stats"){
            double usage = g_transpositionTable.getUsage();
            size_t size = g_transpositionTable.getSize();
            std::cout << requestId << " size:" << size << " usage:" << usage << "%" << std::endl;


            } else if (command == "debug_hash") {
                // Test hash consistency
                std::cerr << "=== HASH DEBUG TEST ===" << std::endl;
                
                // Calculate initial hash
                U64 originalHash = game.board.getHash();
                std::cerr << "Original hash: " << std::hex << originalHash << std::dec << std::endl;
                
                // Make a move and unmake it
                MovesStruct moves = game.generateAllLegalMoves();
                if (moves.getNumMoves() > 0) {
                    Move testMove = moves.getMove(0);
                    std::cerr << "Making move: " << testMove.toString() << std::endl;
                    
                    game.pushMove(testMove);
                    U64 afterMoveHash = game.board.getHash();
                    std::cerr << "After move hash: " << std::hex << afterMoveHash << std::dec << std::endl;
                    
                    game.popMove();
                    U64 afterUndoHash = game.board.getHash();
                    std::cerr << "After undo hash: " << std::hex << afterUndoHash << std::dec << std::endl;
                    
                    if (originalHash == afterUndoHash) {
                        std::cerr << "✅ Hash consistency OK" << std::endl;
                    } else {
                        std::cerr << "❌ Hash consistency BROKEN!" << std::endl;
                    }
                }
                
                std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_zobrist") {
    MoveTables& mt = MoveTables::instance();
    
    std::cerr << "=== ZOBRIST DEBUG ===" << std::endl;
    std::cerr << "Sample zobrist values:" << std::endl;
    std::cerr << "Piece[0][0]: " << std::hex << mt.zobristTable[0][0] << std::dec << std::endl;
    std::cerr << "Piece[1][1]: " << std::hex << mt.zobristTable[1][1] << std::dec << std::endl;
    std::cerr << "SideToMove: " << std::hex << mt.zobristSideToMove << std::dec << std::endl;
    std::cerr << "Castling[0]: " << std::hex << mt.zobristCastling[0] << std::dec << std::endl;
    std::cerr << "EnPassant[0]: " << std::hex << mt.zobristEnPassant[0] << std::dec << std::endl;
    
    if (mt.zobristTable[0][0] == 0 && mt.zobristTable[1][1] == 0) {
        std::cerr << "❌ ZOBRIST TABLES ARE ALL ZEROS!" << std::endl;
    } else {
        std::cerr << "✅ Zobrist tables look OK" << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;


    } else if (command == "debug_tt_detailed") {
    std::cerr << "=== DETAILED TT DEBUG ===" << std::endl;
    
    // Clear TT first
    g_transpositionTable.clear();
    std::cerr << "TT cleared" << std::endl;
    
    U64 testHash = game.board.getHash();
    Move testMove(0, 1, -1, nPawns, nEmpty);
    
    std::cerr << "Test hash: " << std::hex << testHash << std::dec << std::endl;
    
    // Store with different parameters
    g_transpositionTable.store(testHash, 150, 4, TT_EXACT, testMove);
    std::cerr << "Stored: hash=" << std::hex << testHash << std::dec << " score=150 depth=4 flag=EXACT" << std::endl;
    
    // Try different probe parameters
    int score;
    Move move;
    
    // Test 1: Same parameters
    bool found1 = g_transpositionTable.probe(testHash, -1000, 1000, 4, score, move);
    std::cerr << "Probe (same depth 4): " << (found1 ? "FOUND" : "NOT FOUND") << std::endl;
    if (found1) std::cerr << "  Retrieved score: " << score << std::endl;
    
    // Test 2: Lower depth
    bool found2 = g_transpositionTable.probe(testHash, -1000, 1000, 3, score, move);
    std::cerr << "Probe (lower depth 3): " << (found2 ? "FOUND" : "NOT FOUND") << std::endl;
    if (found2) std::cerr << "  Retrieved score: " << score << std::endl;
    
    // Test 3: Higher depth  
    bool found3 = g_transpositionTable.probe(testHash, -1000, 1000, 5, score, move);
    std::cerr << "Probe (higher depth 5): " << (found3 ? "FOUND" : "NOT FOUND") << std::endl;
    if (found3) std::cerr << "  Retrieved score: " << score << std::endl;
    
    // Test 4: Different bounds
    bool found4 = g_transpositionTable.probe(testHash, -500, 500, 4, score, move);
    std::cerr << "Probe (narrow bounds): " << (found4 ? "FOUND" : "NOT FOUND") << std::endl;
    if (found4) std::cerr << "  Retrieved score: " << score << std::endl;
    
    // Test 5: Wide bounds
    bool found5 = g_transpositionTable.probe(testHash, -30000, 30000, 4, score, move);
    std::cerr << "Probe (wide bounds): " << (found5 ? "FOUND" : "NOT FOUND") << std::endl;
    if (found5) std::cerr << "  Retrieved score: " << score << std::endl;
    
    std::cout << requestId << " debug_complete" << std::endl;




    } else if (command == "debug_search_tree") {
    std::string moveStr;
    iss >> moveStr;
    
    std::cerr << "=== SEARCH TREE DEBUG: " << moveStr << " ===" << std::endl;
    
    // Find the move
    MovesStruct moves = game.generateAllLegalMoves();
    for (int i = 0; i < moves.getNumMoves(); ++i) {
        Move move = moves.getMove(i);
        if (move.toString() == moveStr) {
            std::cerr << "Analyzing move: " << move.toString() << std::endl;
            
            // Test at different depths
            for (int testDepth = 1; testDepth <= 4; ++testDepth) {
                game.pushMove(move);
                
                resetSearchStats();
                g_searchStartTime = std::chrono::steady_clock::now();
                g_timeLimit = 10000; // 10 seconds
                
                int score = -alphabeta(-30000, 30000, testDepth, game);
                
                std::cerr << "Depth " << testDepth << ": " << score 
                         << " (nodes: " << g_nodeCount << ")" << std::endl;
                
                game.popMove();
            }
            break;
        }
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_tt_consistency") {
    std::cerr << "=== TT CONSISTENCY TEST ===" << std::endl;
    
    // Clear TT
    g_transpositionTable.clear();
    
    U64 originalHash = game.board.getHash();
    std::cerr << "Original hash: " << std::hex << originalHash << std::dec << std::endl;
    
    // Store something in TT
    Move testMove(0, 8, -1, nPawns, nEmpty);
    g_transpositionTable.store(originalHash, 150, 3, TT_EXACT, testMove);
    std::cerr << "Stored: score=150, depth=3" << std::endl;
    
    // Try to retrieve it
    int retrievedScore;
    Move retrievedMove;
    bool found = g_transpositionTable.probe(originalHash, -1000, 1000, 3, retrievedScore, retrievedMove);
    
    std::cerr << "Retrieved: " << (found ? "SUCCESS" : "FAILED") << std::endl;
    if (found) {
        std::cerr << "Score: " << retrievedScore << std::endl;
        std::cerr << "Move: " << retrievedMove.toString() << std::endl;
    }
    
    // Test with different depth
    bool found2 = g_transpositionTable.probe(originalHash, -1000, 1000, 2, retrievedScore, retrievedMove);
    std::cerr << "Retrieved at depth 2: " << (found2 ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << requestId << " debug_complete" << std::endl;


} else if (command == "debug_tactical_position") {
    std::cerr << "=== TACTICAL POSITION TEST ===" << std::endl;
    
    // Test a position where a piece can be captured
    // Find captures available
    MovesStruct captures = game.generateAllLegalMoves(true);
    std::cerr << "Available captures: " << captures.getNumMoves() << std::endl;
    
    for (int i = 0; i < captures.getNumMoves(); ++i) {
        Move move = captures.getMove(i);
        std::cerr << "\nTesting capture: " << move.toString() << std::endl;
        
        int beforeEval = evaluateBoard(game.board);
        
        game.pushMove(move);
        int afterEval = evaluateBoard(game.board);
        
        // Test if opponent can recapture
        MovesStruct responses = game.generateAllLegalMoves(true);
        std::cerr << "  Opponent responses: " << responses.getNumMoves() << std::endl;
        
        int bestResponse = afterEval;
        for (int j = 0; j < responses.getNumMoves(); ++j) {
            Move response = responses.getMove(j);
            game.pushMove(response);
            int responseEval = evaluateBoard(game.board);
            if (responseEval > bestResponse) {
                bestResponse = responseEval;
                std::cerr << "    Best response: " << response.toString() 
                         << " (eval: " << responseEval << ")" << std::endl;
            }
            game.popMove();
        }
        
        game.popMove();
        
        std::cerr << "  Net gain: " << (beforeEval - bestResponse) << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_quiescence_disabled") {
    std::cerr << "=== QUIESCENCE DISABLED TEST ===" << std::endl;
    
    // Test current position with and without quiescence
    int staticEval = evaluateBoard(game.board);
    std::cerr << "Static eval: " << staticEval << std::endl;
    
    // Temporarily enable quiescence by calling it directly
    int quiesceEval = quiescenceSearch(-30000, 30000, game, 0);
    std::cerr << "Quiescence eval: " << quiesceEval << std::endl;
    std::cerr << "Difference: " << (quiesceEval - staticEval) << std::endl;
    
    if (abs(quiesceEval - staticEval) > 50) {
        std::cerr << "❌ LARGE DIFFERENCE - Quiescence search matters here!" << std::endl;
    } else {
        std::cerr << "✅ Small difference - Position is quiet" << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;

} else if (command == "debug_moves") {
    std::cerr << "=== ALL LEGAL MOVES ===" << std::endl;
    
    MovesStruct moves = game.generateAllLegalMoves();
    std::cerr << "Total legal moves: " << moves.getNumMoves() << std::endl;
    
    for (int i = 0; i < moves.getNumMoves(); ++i) {
        Move move = moves.getMove(i);
        std::cerr << i << ": " << move.toString() << std::endl;
    }
    
    std::cout << requestId << " debug_complete" << std::endl;
// ...existing code...
} else if (command == "debug_best_move") {

    MovesStruct moves = game.generateAllLegalMoves();
    std::cerr << "=== BEST MOVE DEBUG ===" << std::endl;
    std::cerr << "Available moves: " << moves.getNumMoves() << std::endl;

    // show engine state
    std::cerr << "Side to move: " << ((game.board.gameInfo & 1) ? "white" : "black") << std::endl;
    std::cerr << "g_timeLimit (ms): " << g_timeLimit << " g_nodeCount(start): " << g_nodeCount << std::endl;
    std::cerr << "TT probes/hits (start): " << g_ttProbes << " / " << g_ttHits << std::endl;

    int rootDepth = MAX_SEARCH_DEPTH; // match searchAtDepth usage
    resetSearchStats();
    g_searchStartTime = std::chrono::steady_clock::now();

    Move bestMove = searchAtDepth(game, rootDepth);
    std::cerr << "searchAtDepth(" << rootDepth << ") returned: " << bestMove.toString() << std::endl;

    // Detailed per-move scoring using same convention as searchAtDepth
    std::vector<std::pair<int, Move>> allScores;
    int nodeBefore = g_nodeCount;
    U64 lastHash = game.board.getHash();

    for (int i = 0; i < moves.getNumMoves(); ++i) {
        Move move = moves.getMove(i);

        // quick repetition check (if API available)
        bool immediateRepetition = false;
        if (/* game has history API */ false) {
            // placeholder - replace with actual repetition call if present:
            // immediateRepetition = game.wouldBeRepetition(move);
        }

        game.pushMove(move);
        int score = -alphabeta(-30000, 30000, rootDepth - 1, game);
        game.popMove();

        allScores.push_back({score, move});
        std::cerr << "Move " << i << ": " << move.toString()
                  << " Score: " << score
                  << (immediateRepetition ? " [repetition]" : "") << std::endl;
    }

    // TT stats & nodes used by this debug pass
    int nodesUsed = g_nodeCount - nodeBefore;
    std::cerr << "nodes used by debug pass: " << nodesUsed << "  total nodes: " << g_nodeCount << std::endl;
    std::cerr << "TT probes/hits (end): " << g_ttProbes << " / " << g_ttHits << std::endl;

    // sort and display top moves
    std::sort(allScores.begin(), allScores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::cerr << "\n=== TOP 8 MOVES ===" << std::endl;
    for (int i = 0; i < std::min(8, (int)allScores.size()); ++i) {
        std::cerr << i + 1 << ". " << allScores[i].second.toString()
                  << " (" << allScores[i].first << ")" << std::endl;
    }

    // detect simple back-and-forth: check if engine's chosen move is the inverse of opponent's last move
    if (moves.getNumMoves() > 0) {
        // try to detect last move and inverse (replace with your API if needed)
        // Example pseudo-check: if (bestMove == inverseOf(lastMove)) warn
        std::cerr << "Note: if you see the same corner rook moves repeated, add repetition detection in search." << std::endl;
    }

    std::cout << requestId << " debug_complete" << std::endl;
} else {
            std::cout << requestId << "unknown command" << std::endl;
        }



    

    }
    return 0;

}