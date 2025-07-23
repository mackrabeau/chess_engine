import 'bootstrap/dist/css/bootstrap.css'
import ChessBoard from './components/ChessBoard';
import './App.css';

function App() {

  return (
    <>
        <div className="intro-text">
            <h1 className="chessboard-title">Chess Engine</h1>
            <p>chess engine using c++. description here</p>
        </div>
        <div className="chessboard-wrapper">
            <ChessBoard />
        </div>
    </>
 
  );
}

export default App

