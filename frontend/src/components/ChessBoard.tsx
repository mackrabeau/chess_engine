import { Chessboard } from 'react-chessboard';
// import { useState, useRef } from 'react';
import { useState, useEffect } from 'react';

interface MoveHistoryItem {
  moveNumber: number;
  whiteMove: string;
  blackMove?: string;
  whiteEval?: number;
  blackEval?: number;
  fen: string;
}

function ChessBoard() {
    const initialPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // Standard starting position

    const [position, setPosition] = useState(initialPosition);
    const [isGameOver, setIsGameOver] = useState(false);
    const [isLoading, setIsLoading] = useState(false);
    const [searchDepth, setSearchDepth] = useState(0);

    const [currentEval, setCurrentEval] = useState(0);
    const [moveHistory, setMoveHistory] = useState<MoveHistoryItem[]>([]);
    const [currentMoveNumber, setCurrentMoveNumber] = useState(1);
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);

    const [autoPlay, setAutoPlay] = useState(false);
    const [gameResult, setGameResult] = useState<string | null>(null);
    const [autoPlayDelay] = useState(600);

    const EvalBar = ({ evaluation }: { evaluation: number }) => {
        // Convert centipawn evaluation to percentage (clamped between -10 and +10 pawns)
        const clampedEval = Math.max(-1000, Math.min(1000, evaluation));
        const percentage = 50 + (clampedEval / 1000) * 50; // 0-100%
        
        const getEvalText = (evaluationValue: number) => {
            const score = Math.abs(evaluationValue) / 100;
            if (score >= 10) return `±${Math.floor(score)}`;
            if (score >= 1) return score.toFixed(1);
            return score.toFixed(2);
        };

    return (
        <div className="eval-bar-container">
            <div 
                className="eval-bar"
                style={{
                    // For horizontal bars on mobile
                    '--eval-width': `${percentage}%`
                } as React.CSSProperties}
            >
                <div 
                    className="eval-bar-fill" 
                    style={{ height: `${percentage}%` }}
                />
                <div className="eval-text">
                    {getEvalText(evaluation)}
                </div>
            </div>
        </div>
    );
    };

    const MoveHistory = () => {
        return (
            <div className="move-history">
                <h3>Move History</h3>
                <div className="move-history-content">
                    {moveHistory.length === 0 ? (
                        <div className="no-moves">No moves yet</div>
                    ) : (
                        moveHistory.map((item, index) => (
                            <div key={index} className="move-row">
                                <span className="move-number">{item.moveNumber}.</span>
                                <span className="white-move">
                                    {item.whiteMove}
                                    {item.whiteEval !== undefined && (
                                        <span className="move-eval">
                                            ({item.whiteEval > 0 ? '+' : ''}{(item.whiteEval / 100).toFixed(2)})
                                        </span>
                                    )}
                                </span>
                                {item.blackMove && item.blackMove !== "" && (
                                    <span className="black-move">
                                        {item.blackMove}
                                        {item.blackEval !== undefined && (
                                            <span className="move-eval">
                                                ({item.blackEval > 0 ? '+' : ''}{(item.blackEval / 100).toFixed(2)})
                                            </span>
                                        )}
                                    </span>
                                )}
                                {(!item.blackMove || item.blackMove === "") && (
                                    <span className="black-move placeholder">
                                        ...
                                    </span>
                                )}
                            </div>
                        ))
                    )}
                </div>
            </div>
        );
    };

    const getPositionEval = async () => {
        try {
            const response = await fetch('http://localhost:3001/api/getEval', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
            });
            const data = await response.json();
            return data.evaluation || 0;
        } catch (error) {
            console.error('Error getting evaluation:', error);
            return 0;
        }
    };

    const fetchCurrentPosition = async () => {
        try {
            const response = await fetch('http://localhost:3001/api/getPosition', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
            });
            const data = await response.json();
            if (data.position) {
                console.log('Fetched position from engine:', data.position);
                setPosition(data.position);
                
                // Parse turn from FEN
                const fenParts = data.position.split(' ');
                if (fenParts.length >= 2) {
                    setIsWhiteTurn(fenParts[1] === 'w');
                }

                // Also fetch the current evaluation
                const evaluation = await getPositionEval();
                setCurrentEval(evaluation);
            }
        } catch (error) {
            console.error('Error fetching position from engine:', error);
            // Fallback to initial position if fetch fails
        }
    };

    const addMoveToHistory = async (move: string, isWhite: boolean, newPosition: string) => {
    console.log(`=== ADDING MOVE ===`);
    console.log(`Move: ${move}`);
    console.log(`IsWhite: ${isWhite}`);
    console.log(`Current moveNumber: ${currentMoveNumber}`);
    console.log(`Current history length: ${moveHistory.length}`);
    
        const evaluation = await getPositionEval();
        setCurrentEval(evaluation);

        setMoveHistory(prev => {
            const newHistory = [...prev];
            
            if (isWhite) {
                // White move - start new entry
                newHistory.push({
                    moveNumber: currentMoveNumber,
                    whiteMove: move,
                    blackMove: "",
                    whiteEval: evaluation,
                    fen: newPosition
                });
            } else {
                // Black move - complete current entry
                if (newHistory.length > 0) {
                    const lastEntry = newHistory[newHistory.length - 1];
                    lastEntry.blackMove = move;
                    lastEntry.blackEval = evaluation;
                    lastEntry.fen = newPosition;
                }
                setCurrentMoveNumber(currentMoveNumber + 1)
            }
            
            return newHistory;
        });  
    };

    const pollSearchDepth = async () => {
      console.log("pollSearchDepth called, isLoading:", isLoading); // Add this

      if (!isLoading) return;
      try {
        const response = await fetch('http://localhost:3001/api/searchDepth');
        const data = await response.json();
        setSearchDepth(data.depth);

      } catch (error) {
        console.error('Error polling search depth:', error);
      }
    };

    async function getBestMove() {
      if (isGameOver || isLoading) return;

      try {
          setSearchDepth(0);
        
          const response = await fetch('http://localhost:3001/api/getBestMove', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
        });

        const data = await response.json();

        if (!response.ok) {
            console.error('API Error:', data);
            throw new Error(data.message || data.error || 'Unknown error');
        }
        
        return data.bestMove; // example: "e2e4"

      } catch (err) {
        console.error("getBestMove failed:", err);
      }
    }

    // Add effect to poll depth while thinking
    useEffect(() => {
      let interval: ReturnType<typeof setInterval>;
      
      if (isLoading) {
        interval = setInterval(pollSearchDepth, 200); // Poll every 200ms
      }
      
      return () => {
        if (interval) clearInterval(interval);
      };
    }, [isLoading]);

    // Fetch initial position from engine on component mount
    useEffect(() => {
        fetchCurrentPosition();
    }, []);

    const formatMoveNotation = (from: string, to: string, piece: string | null, promoPiece: string | null): string => {
        if (!promoPiece) {
          promoPiece = "";
        }

        // Simple algebraic notation (can be enhanced)
        const pieceSymbols: { [key: string]: string } = {
            'k': 'K', 'q': 'Q', 'r': 'R', 'b': 'B', 'n': 'N'
        };
        
        if (!piece) return `${from}-${to}`;
        

        const pieceChar = piece.toLowerCase();
        if (pieceChar === 'p') {
            return to + promoPiece; // Pawn moves just show destination
        }

        return `${pieceSymbols[pieceChar] || ''}${to + promoPiece}`;
    };

    // const formatMoveNotation = (moveString: string, piece: string | null): string => {
    //     const from = moveString.slice(0, 2);
    //     const to = moveString.slice(2, 4);
    //     const promoPiece = moveString.length > 4 ? moveString[4] : ""; // Check for promotion piece

    //     // Simple algebraic notation (can be enhanced)
    //     const pieceSymbols: { [key: string]: string } = {
    //         'k': 'K', 'q': 'Q', 'r': 'R', 'b': 'B', 'n': 'N'
    //     };
        
    //     if (!piece) return `${from}-${to}`;
        
    //     const pieceChar = piece.toLowerCase();
    //     if (pieceChar === 'p') {
    //         return to + promoPiece; // Pawn moves just show destination
    //     }
    //     return `${pieceSymbols[pieceChar] || ''}${to + promoPiece}`;
    // };


    async function makeMoveExplicit(moveString: string, isWhiteMove: boolean, currentPosition = position) {
      try {
        const from = moveString.slice(0, 2);
        const to = moveString.slice(2, 4);
        const promoPiece = moveString.length > 4 ? moveString[4] : "";

        const piece = getPieceAtSquare(from, currentPosition);
        const moveNotation = formatMoveNotation(from, to, piece, promoPiece);

        console.log(`Making move: ${moveNotation}, isWhiteMove: ${isWhiteMove}`);

        const response = await fetch('http://localhost:3001/api/makeMove', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ 
            fen: currentPosition,
            from, 
            to,
            promoPiece
          }),
        });

        const data = await response.json();

        if (!response.ok) {
          console.error('Move API Error:', data);
          return {success: false, newPosition: null};
        }

        if (data.newPosition && data.newPosition.startsWith("error:")) {
          console.log('Illegal move attempted:', data.newPosition);
          return {success: false, newPosition: null};
        }

        if (data.newPosition) {
          setPosition(data.newPosition);

          await addMoveToHistory(moveNotation, isWhiteMove, data.newPosition);
          setIsWhiteTurn(!isWhiteMove);

          return {success: true, newPosition: data.newPosition};
        }

        return {success: false, newPosition: null};

      } catch (err) {
        console.error("Error making move:", err);
        return {success: false, newPosition: null};
      }
    }

    async function resetGame() {
      try {
        setIsLoading(true);

        const response = await fetch('http://localhost:3001/api/reset', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
        });

        const data = await response.json();

        if (!response.ok) {
          console.error('Reset API Error:', data);
          throw new Error(data.message || data.error || 'Unknown error');
        }

        // Fetch current position from engine after reset
        await fetchCurrentPosition();
        setIsGameOver(false);
        setGameResult(null);
        setSearchDepth(0);
        setCurrentEval(0);
        setMoveHistory([]);
        setCurrentMoveNumber(1);

        console.log("Game reset!")

      } catch (err) {
        console.error("Error resetting game:", err);
      } finally {
        setIsLoading(false);
      }
    }

    async function getGameState() {
      try {
        const response = await fetch('http://localhost:3001/api/getGameState', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
        });

        const data = await response.json();

        if (data && data.result) {
          const r = data.result;
          const isOver = r !== "ongoing";
          setIsGameOver(isOver);
          if (!isOver) {
            setGameResult(null);
          } else {
            // Map engine result codes to user-friendly labels
            if (r === "checkmate") {
              setGameResult("Checkmate");
            } else if (r === "stalemate" || r.startsWith("draw") || r === "draw_repetition" || r === "draw_50_move" || r === "draw_insufficient_material") {
              setGameResult("Draw");
            } else {
              setGameResult(r); // fallback: show raw string
            }
          }
        }

      } catch (err) {
        console.error("Error checking game state:", err);
      }
    }

    useEffect(() => {
      let cancelled = false;

      async function runAutoPlay() {
        while (!cancelled && autoPlay && !isGameOver) {
          // wait if engine already thinking
          if (isLoading) {
            await new Promise(res => setTimeout(res, 100));
            continue;
          }

          // determine side to move from FEN
          const fenParts = position.split(' ');
          const sideIsWhite = fenParts[1] === 'w';

          try {
            // ask engine for best move (optionally pass current FEN if server supports it)
            setIsLoading(true);
            // If your backend expects a fen parameter, modify getBestMove to accept it.
            const bestMove = await getBestMove(); // assume server uses internal position
            if (!bestMove) break;

            // play the move for the correct side
            await makeMoveExplicit(bestMove, sideIsWhite, position);

            // update game state / evaluation / history
            await getGameState();

          } catch (err) {
            console.error("Autoplay error:", err);
            break;
          } finally {
            setIsLoading(false);
          }

          // delay between moves
          await new Promise(res => setTimeout(res, autoPlayDelay));
        }
      }

      if (autoPlay && !cancelled) runAutoPlay();

      return () => { cancelled = true; };
    }, [autoPlay, position, isGameOver, isLoading, autoPlayDelay]);

           

    const isPromotionMove = (from: string, to: string): boolean => {
      // Get piece from current position string
      const piece = getPieceAtSquare(from, position);
      
      if (piece?.toLowerCase() === 'p') { // It's a pawn
        const toRank = parseInt(to[1]);
        
        // White pawn to rank 8 or Black pawn to rank 1
        if ((piece === 'P' && toRank === 8) || (piece === 'p' && toRank === 1)) {
          return true;
        }
      }
      
      return false;
    };


    const getPieceAtSquare = (square: string, fen: string): string | null => {
      const file = square.charCodeAt(0) - 'a'.charCodeAt(0); // 0-7
      const rank = parseInt(square[1]) - 1; // 0-7
      
      const boardPart = fen.split(' ')[0];
      const ranks = boardPart.split('/');
      
      // FEN ranks are from 8 to 1, so reverse the rank index
      const fenRank = ranks[7 - rank];
      
      let fileIndex = 0;
      for (let i = 0; i < fenRank.length; i++) {
        const char = fenRank[i];
        
        if (char >= '1' && char <= '8') {
          // Empty squares
          fileIndex += parseInt(char);
        } else {
          // Piece
          if (fileIndex === file) {
            return char;
          }
          fileIndex++;
        }
      }
      
      return null;
    };

    function onPieceDrop({ sourceSquare, targetSquare }: { 
      sourceSquare: string; 
      targetSquare: string | null; 
    }) {

      if (autoPlay) return false;

      if (!targetSquare || isLoading || isGameOver) {
        return false;
      }

      setIsLoading(true);

      let moveString = sourceSquare + targetSquare;
      if (isPromotionMove(sourceSquare, targetSquare)) {
        moveString += 'q';
      }
      
      try {
        if (sourceSquare === targetSquare) {
          setIsLoading(false);
          return false;
        }

        // ✅ Track turn explicitly to avoid React state async issues
        let currentPlayerIsWhite = isWhiteTurn;
        console.log(`User move: ${moveString}, currentPlayerIsWhite: ${currentPlayerIsWhite}`);

        makeMoveExplicit(moveString, currentPlayerIsWhite)
        .then(async result => {
          console.log("User move result:", result);
          if (result.success && !isGameOver) {
            await getGameState();
            if (!isGameOver) {
              // ✅ Flip turn for engine move
              currentPlayerIsWhite = !currentPlayerIsWhite;
              console.log(`Engine move turn: currentPlayerIsWhite: ${currentPlayerIsWhite}`);
              
              const bestMove = await getBestMove();
              if (bestMove) {
                const engineMove = await makeMoveExplicit(bestMove, currentPlayerIsWhite, result.newPosition);
                if (engineMove.success) {
                  await getGameState();
                }
              }
            }
          }
          setIsLoading(false);
        })
        .catch(error => {
          console.error("Error making move:", error);
          setIsLoading(false);
        });

        return true;
      } catch {
        setIsLoading(false);
        return false;
      }
    }

    return (

  <div className="chess-container">
        <div className="game-area">
            <div className="eval-section">
                <EvalBar evaluation={currentEval} />
            </div>

            <div className="board-section">
                <Chessboard options={{
                    position: position,
                    onPieceDrop,
                    // animationDurationInMs: 300,
                    id: "main",
                }} /> 

                <button 
                    onClick={resetGame}
                    disabled={isLoading}
                    className="reset-button"> 
                    New Game
                </button>

                <label style={{display:'inline-block', marginLeft:12}}>
                 Auto-play
                 <input
                   type="checkbox"
                   checked={autoPlay}
                   onChange={(e) => setAutoPlay(e.target.checked)}
                   style={{marginLeft:6}}
                 />

               </label>
                <div className={`loading-indicator ${isLoading ? '' : 'hidden'}`}>
                    Thinking... <br />
                    {searchDepth > 0 && <span> (Depth {searchDepth})</span>}
                </div>
                {/* {isLoading && (
                    <div className="loading-indicator">
                        Thinking... <br />
                        {searchDepth > 0 && <span> (Depth {searchDepth})</span>}
                    </div>
                )} */}
                {isGameOver && (
                    <div className="game-over-indicator">
                        {gameResult ? gameResult : "Game Over"}
                    </div>    
                )}

            </div>
            <div className="history-section">
                <MoveHistory />
            </div>
        </div>
    </div>
    );

  }

export default ChessBoard;

