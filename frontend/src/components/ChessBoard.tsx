import { Chessboard } from 'react-chessboard';
// import { useState, useRef } from 'react';
import { useState } from 'react';

function ChessBoard() {
    const initialPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // initial FEN position
    // const initialPosition = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    const [position, setPosition] = useState(initialPosition);
    const [isGameOver, setIsGameOver] = useState(false);
    const [isLoading, setIsLoading] = useState(false);


    // // Function to extract en passant square from FEN
    // function getEnPassantSquare(fen: string): string | null {
    //     const fenParts = fen.split(' ');
    //     if (fenParts.length >= 4) {
    //         const epSquare = fenParts[3];
    //         return epSquare === '-' ? null : epSquare;
    //     }
    //     return null;
    // }

    // async function getRandomMove(currentPosition: string) {
    //   if (isGameOver || isLoading) return;

    //   try {
    //       const response = await fetch('http://localhost:3001/api/getRandomMove', {
    //         method: 'POST',
    //         headers: { 'Content-Type': 'application/json' },
    //         body: JSON.stringify({ fen: currentPosition}),
    //     });

    //     const data = await response.json();
    //     return data.randomMove; // example: "e2e4"

    //   } catch (err) {
    //     console.error("Engine error:", err);
    //   }
    // }

    async function getBestMove(currentPosition: string) {
      if (isGameOver || isLoading) return;

      try {
          const response = await fetch('http://localhost:3001/api/getBestMove', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ fen: currentPosition}),
        });

        const data = await response.json();
        return data.bestMove; // example: "e2e4"

      } catch (err) {
        console.error("Engine error:", err);
      }
    }

    async function makeMove(moveString: string, currentPosition = position) {
      try {
        const from = moveString.slice(0, 2);
        const to = moveString.slice(2, 4);

        const response = await fetch('http://localhost:3001/api/makeMove', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ 
            fen: currentPosition,
            from,
            to, 
          }),
        });

        const data = await response.json();

        if (data.newPosition) {
          setPosition(data.newPosition);
          getGameState(data.newPosition);
          return {success: true, newPosition: data.newPosition};
        }

        return {success: false, newPosition: null};

      } catch (err) {
        console.error("Error making move:", err);
        return {success: false, newPosition: null};
      }
    }

    async function getGameState(fen: string) {
      try {
        const response = await fetch('http://localhost:3001/api/getGameState', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ fen: fen }),
        });

        const data = await response.json();
        
        if (data.isGameOver) {
          setIsGameOver(true);
          alert("Game Over! " + data.result);
        }

      } catch (err) {
        console.error("Error checking game state:", err);
      }
    }

    function onPieceDrop({ sourceSquare, targetSquare }: { 
      sourceSquare: string; 
      targetSquare: string | null; 
      // piece: { pieceType: string; isSparePiece: boolean } 
    }) {

      if (!targetSquare || isLoading || isGameOver) {
        return false;
      }

      setIsLoading(true);
      
      try {
        const moveString = sourceSquare + targetSquare

        makeMove(moveString)
        .then(async result => {
          console.log("Move result:", result);
          if (result.success && !isGameOver) {
            // const randomMove = await getRandomMove(result.newPosition);
            // if (randomMove) {
            //   await makeMove(randomMove, result.newPosition);
            // }
            const bestMove = await getBestMove(result.newPosition);

            if (bestMove) {
              await makeMove(bestMove, result.newPosition);
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
      <div>
        <Chessboard options={{
            position: position,
            onPieceDrop,
            // animationDurationInMs: 300,
            id: "main",
        }} />
        {isLoading && <div className="loading-indicator">Thinking...</div>}
        {isGameOver && <div className="loading-indicator">Game Over</div>}
      </div>

    );
  }

export default ChessBoard;

