#include "Board.h"
#include "BoardUtils.h"
#include "Square.h"
#include "PieceLogic.h"
#include "GraphicsManager.h"

#include <iostream>
#include <fstream>

Board::Board():board_width(8),board_height(8),playerToMove(White),
              turn(1),score(0)
{
  for(int x=0; x<board_width; x++)
  {
    for(int y=0; y<board_height; y++)
    {
      board[x][y].setColor( ((x+y)%2==0?BlackSquare:WhiteSquare) );
    }
  }

  setupDefault();
  }

void Board::setupDefault()
{
  // set owners (and pawns)
  for(int x=0; x<board_width; x++)
  {
    board[x][0].setPlayer(White);
    board[x][1].setPlayer(White);

    board[x][1].setPiece(Pawn);
    board[x][6].setPiece(Pawn);

    board[x][6].setPlayer(Black);
    board[x][7].setPlayer(Black);
  }

  board[0][0].setPiece(Rook);
  board[0][7].setPiece(Rook);

  board[1][0].setPiece(Knight);
  board[1][7].setPiece(Knight);

  board[2][0].setPiece(Bishop);
  board[2][7].setPiece(Bishop);

  board[3][0].setPiece(Queen);
  board[3][7].setPiece(Queen);

  board[4][0].setPiece(King);
  board[4][7].setPiece(King);

  board[5][0].setPiece(Bishop);
  board[5][7].setPiece(Bishop);

  board[6][0].setPiece(Knight);
  board[6][7].setPiece(Knight);

  board[7][0].setPiece(Rook);
  board[7][7].setPiece(Rook);
}

void Board::setupFromFile(std::ifstream& in)
{
  clearBoard();

  char x_in;
  char y_in;
  char pl_in;
  char pi_in;

  int x;
  int y;

  while(in >> x_in >> y_in >> pl_in >> pi_in)
  {
    x = x_in - 'a';
    y = y_in - '1';

    if(!areValidCoordinates(x,y))
    {
      continue;
    }

    if(pl_in == 'w')
    {
      setPlayer(x,y,White);
    }
    else if(pl_in == 'b')
    {
      setPlayer(x,y,Black);
    }
    else
    {
      continue;
    }

    switch(pi_in)
    {
      case 'p':
        setPiece(x,y,Pawn);
        break;
      case 'n':
        setPiece(x,y,Knight);
        break;
      case 'b':
        setPiece(x,y,Bishop);
        break;
      case 'r':
        setPiece(x,y,Rook);
        break;
      case 'q':
        setPiece(x,y,Queen);
        break;
      case 'k':
        setPiece(x,y,King);
        break;
    }
  }
}

void Board::clearBoard()
{
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      setPiece(x,y,Empty);
      setPlayer(x,y,None);
    }
  }
}

Board::Board(const Board& other):board_width(8),board_height(8)
{
  makeIntoCopyOf(other);
}

Board::~Board()
{
}

void Board::makeIntoCopyOf(const Board& other)
{
  playerToMove = other.playerToMove; 
  turn = other.turn;
  score = other.score;

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      board[x][y].makeIntoCopyOf(other.board[x][y]); 
    }
  }
}

void Board::render() const
{
  if(true)//gm != nullptr)
  {
    GraphicsManager::renderBoard(this);
  }
  else
  {
    for(int y=board_height-1; y>=0; y--)
    {
      std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
      std::cout << y+1 << " ";
      for(int x=0; x<board_width; x++)
      {
        std::cout << (board[x][y].getColor()==WhiteSquare?"|(":"| ");
        board[x][y].render();
        std::cout << (board[x][y].getColor()==WhiteSquare?")":" ");
      }
      std::cout << "|" << std::endl;
    }
    std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << "    ";
    for(int x=0; x<board_width; x++)
    {
      std::cout << (char)('a' + x);
      std::cout << "   ";
    }
    std::cout << std::endl;
  }

  /*
  if(playerToMove == White)
  {
    std::cout << "  White to move" << std::endl;
  }
  else
  {
    std::cout << "  Black to move" << std::endl;
  }
  */
}

bool Board::move(std::string notation)
{
  // replace castling notation with consistant-with-everything-else notation

  if(notation.compare("o-o") == 0
  || notation.compare("O-O") == 0
  || notation.compare("0-0") == 0)
  {
    if(playerToMove == White)
    {
      notation = "e1 g1";
    }
    else
    {
      notation = "e8 g8";
    }
  }

  if(notation.compare("o-o-o") == 0
  || notation.compare("O-O-O") == 0
  || notation.compare("0-0-0") == 0)
  {
    if(playerToMove == White)
    {
      notation = "e1 c1";
    }
    else
    {
      notation = "e8 c8";
    }
  }

  SpecialMove sm = NoMove;
  if(parseNotation(notation,move_begin,move_end,&sm))
  {
    Square::movePiece(at( move_begin[0], move_begin[1] ),
                      at(   move_end[0],   move_end[1] ));

    performSpecialMove(move_begin,move_end,sm); 

    switchPlayer();
    return true;
  }

  return false;
}

bool Board::move(const Move& m)
{
  int move_begin[2] = { m.start_x , m.start_y };
  int move_end[2]   = { m.end_x   , m.end_y   };

  SpecialMove sm = NoMove;
  if(PieceLogic::isMoveValid(this,move_begin,move_end,m.promote,&sm))
  {
    addScore(BoardUtils::getPieceScore( getPiece(move_end) ));

    Square::movePiece(at( move_begin[0], move_begin[1] ),
                      at(   move_end[0],   move_end[1] ));

    // en-passant / castling / promition
    performSpecialMove(move_begin,move_end,sm); 

    switchPlayer();
    return true;
  }

  return false;
}

void Board::performSpecialMove(const int* begin,const int* end,SpecialMove sm)
{
  switch(sm)
  {
    case MovedEnPassant:
      at(end).setEnPassantTurn(getTurn());
      break;
    case CapturedEnPassant:
      at(end[0],begin[1]).setPiece(Empty);
      at(end[0],begin[1]).setPlayer(None);
      at(end[0],begin[1]).setEnPassantTurn(-1);
      break;
    case CastleKingside:
      Square::movePiece( at(7,begin[1]),at(5,begin[1])); 
      break;
    case CastleQueenside:
      Square::movePiece( at(0,begin[1]),at(3,begin[1])); 
      break;
    case PromoteKnight:
      at(end).setPiece(Knight);
      addScore(2);
      break;
    case PromoteBishop:
      at(end).setPiece(Bishop);
      addScore(2);
      break;
    case PromoteRook:
      at(end).setPiece(Rook);
      addScore(4);
      break;
    case PromoteQueen:
      at(end).setPiece(Queen);
      addScore(8);
      break;
  }
}

void Board::switchPlayer()
{
  if(playerToMove == White)
  {
    playerToMove = Black;
  }
  else
  {
    playerToMove = White;
  }
  turn++;
}

bool Board::parseNotation(std::string notation,int* begin,int* end,
                          SpecialMove* sm) const
{
  // notation is in format a1-b2 or a1 b2
  // always 5 characters (except for 0-0 and 0-0-0)
  // a1 is the beginning place for the piece and b2 is the end

  if(notation.size() < 2)
  {
    return false; 
  }

  std::string buffer;

  begin[0] = (notation.at(0) - 'a');
  begin[1] = (notation.at(1) - '1');
  
  // if the notation is separated by a space, cin reads as 2 separate words, get
  // the next part and put it into buffer
  if(notation.size() == 2)
  {
    std::cin >> buffer;
    end[0]   = (buffer.at(0) - 'a');
    end[1]   = (buffer.at(1) - '1');
  }
  else
  {
    end[0]   = (notation.at(3) - 'a');
    end[1]   = (notation.at(4) - '1');
  }

  Piece promotion = Empty;
  if(notation.size() >= 7)
  {
    switch(notation.at(6))
    {
      case 'n':
      case 'N':
      case 'k':
      case 'K':
        promotion = Knight;
        break;
      case 'b':
      case 'B':
        promotion = Bishop;
        break;
      case 'r':
      case 'R':
        promotion = Rook;
        break;
      case 'q':
      case 'Q':
        promotion = Queen;
        break;
    }
  }
  
  return PieceLogic::isMoveValid(this,begin,end,promotion,sm);
}

Square& Board::at(const int* coord)
{
  return board[ coord[0] ][ coord[1] ];
}

Square& Board::at(int x,int y)
{
  return board[x][y];
}

// getters and setters (coordinates as pair of ints)

Piece Board::getPiece(int x,int y) const
{
  return board[x][y].getPiece();
}

Player Board::getPlayer(int x,int y) const
{
  return board[x][y].getPlayer();
}

Color Board::getColor(int x,int y) const
{
  return board[x][y].getColor();
}

bool Board::getHasMoved(int x,int y) const
{
  return board[x][y].getHasMoved();
}

int Board::getEnPassantTurn(int x,int y) const
{
  return board[x][y].getEnPassantTurn();
}


void Board::setPiece(int x,int y,Piece p)
{
  board[x][y].setPiece(p);
}

void Board::setPlayer(int x,int y,Player p)
{
  board[x][y].setPlayer(p);
}

void Board::setColor(int x,int y,Color c)
{
  board[x][y].setColor(c);
}

void Board::setEnPassantTurn(int x,int y,int t)
{
  board[x][y].setEnPassantTurn(t);
}

// getters and setters (coordinates as array of ints)

Piece Board::getPiece(const int* arr) const
{
  return board[arr[0]][arr[1]].getPiece();
}

Player Board::getPlayer(const int* arr) const
{
  return board[arr[0]][arr[1]].getPlayer();
}

Color Board::getColor(const int* arr) const
{
  return board[arr[0]][arr[1]].getColor();
}

bool Board::getHasMoved(const int* arr) const
{
  return board[arr[0]][arr[1]].getHasMoved();
}

int Board::getEnPassantTurn(const int* arr) const
{
  return board[arr[0]][arr[1]].getEnPassantTurn();
}


void Board::setPiece(const int* arr,Piece p)
{
  board[arr[0]][arr[1]].setPiece(p);
}

void Board::setPlayer(const int* arr,Player p)
{
  board[arr[0]][arr[1]].setPlayer(p);
}

void Board::setColor(const int* arr,Color c)
{
  board[arr[0]][arr[1]].setColor(c);
}

void Board::setEnPassantTurn(const int* arr,int t)
{
  board[arr[0]][arr[1]].setEnPassantTurn(t);
}


bool Board::areValidCoordinates(int x,int y) const
{
  return (x >= 0 && x < board_width && y >= 0 && y < board_height);
}

Player Board::getPlayerToMove() const
{
  return playerToMove;
}

int Board::getTurn() const
{
  return turn;
}

int Board::getScore() const
{
  return score; 
}

bool Board::hasEnded() const
{
  int ws = BoardUtils::getWhiteScore(this);

  // kings are worth 10000, so if white's score is outside this range
  // one side has to be missing their king
  if(ws > 9000 || ws < -9000)
  {
    return true;
  }

  // if the current player has no moves, it's stalemate
  auto moves = BoardUtils::getPossibleMoves(this,playerToMove);

  return moves.empty();
}

void Board::addScore(int s)
{
  if(playerToMove == White)
  {
    score += s;
  }
  else
  {
    score -= s;
  }
}
