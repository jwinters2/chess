#include "Board.h"
#include "Square.h"
#include "PieceLogic.h"
#include "GraphicsManager.h"

#include <iostream>

Board::Board():board_width(8),board_height(8),playerToMove(White),turn(1)
{
  board = new Square**[board_width];
  for(int x=0; x<board_width; x++)
  {
    board[x] = new Square*[board_height];
    for(int y=0; y<board_height; y++)
    {
      board[x][y] = new Square(Empty,None,((x+y)%2==0?BlackSquare:WhiteSquare));
    }
  }

  // set owners (and pawns)
  for(int x=0; x<board_width; x++)
  {
    board[x][0]->setPlayer(White);
    board[x][1]->setPlayer(White);

    board[x][1]->setPiece(Pawn);
    board[x][6]->setPiece(Pawn);

    board[x][6]->setPlayer(Black);
    board[x][7]->setPlayer(Black);
  }

  board[0][0]->setPiece(Rook);
  board[0][7]->setPiece(Rook);

  board[1][0]->setPiece(Knight);
  board[1][7]->setPiece(Knight);

  board[2][0]->setPiece(Bishop);
  board[2][7]->setPiece(Bishop);

  board[3][0]->setPiece(Queen);
  board[3][7]->setPiece(Queen);

  board[4][0]->setPiece(King);
  board[4][7]->setPiece(King);

  board[5][0]->setPiece(Bishop);
  board[5][7]->setPiece(Bishop);

  board[6][0]->setPiece(Knight);
  board[6][7]->setPiece(Knight);

  board[7][0]->setPiece(Rook);
  board[7][7]->setPiece(Rook);
}

Board::~Board()
{
  for(int x=0; x<board_width; x++)
  {
    for(int y=0; y<board_height; y++)
    {
      delete board[x][y];
    }
    delete [] board[x];
  }
  delete [] board;
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
        std::cout << (board[x][y]->getColor()==WhiteSquare?"|(":"| ");
        board[x][y]->render();
        std::cout << (board[x][y]->getColor()==WhiteSquare?")":" ");
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
  // check castling first
  if(notation.compare("0-0") == 0
  || notation.compare("O-O") == 0
  || notation.compare("o-o") == 0)
  {
    // kingside castle
    int kingPos[2] = {4,0};
    int rookPos[2] = {7,0};
    // check both black and white
    for(int i=0; i<=7; i+=7)
    {
      kingPos[1] = i;
      rookPos[1] = i;

      // if neither the king or the rook has moved, and
      if(!at(kingPos)->getHasMoved() && !at(rookPos)->getHasMoved()
      && at(5,i)->getPiece() == Empty && at(6,i)->getPiece() == Empty)
      {
        Square::movePiece(at(kingPos),at(6,i));
        Square::movePiece(at(rookPos),at(5,i));
        switchPlayer();
        return true;
      }
    }
  }

  if(notation.compare("0-0-0") == 0
  || notation.compare("O-O-O") == 0
  || notation.compare("o-o-o") == 0)
  {
    // queenside castle
    int kingPos[2] = {4,0};
    int rookPos[2] = {0,0};
    // check both black and white
    for(int i=0; i<=7; i+=7)
    {
      kingPos[1] = i;
      rookPos[1] = i;

      // if neither the king or the rook has moved, and
      if(!at(kingPos)->getHasMoved() && !at(rookPos)->getHasMoved()
      && at(3,i)->getPiece() == Empty && at(2,i)->getPiece() == Empty
      && at(1,i)->getPiece() == Empty)
      {
        Square::movePiece(at(kingPos),at(2,i));
        Square::movePiece(at(rookPos),at(3,i));
        switchPlayer();
        return true;
      }
    }
  }

  bool enp = false;
  if(parseNotation(notation,move_begin,move_end,&enp))
  {
    Square::movePiece(at( move_begin[0], move_begin[1] ),
                      at(   move_end[0],   move_end[1] ));

    // en-passant
    if(enp)
    {
      Square* toClear = at(move_end[0],move_begin[1]);
      toClear->setPiece(Empty);
      toClear->setPlayer(None);
      toClear->setEnPassantTurn(-1);
    }

    switchPlayer();
    return true;
  }

  return false;
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

bool Board::parseNotation(std::string notation,int* begin,int* end,bool* enp)
{
  // notation is in format a1-b2 or a1 b2
  // always 5 characters (except for 0-0 and 0-0-0)
  // a1 is the beginning place for the piece and b2 is the end

  if(notation.size() != 5 && notation.size() != 2)
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
  
  return PieceLogic::isMoveValid(this,at(begin),begin,end,enp);

  // check for valid coordinates
  if(at(begin) == nullptr || at(end) == nullptr 
  || (begin[0] == end[0] && begin[1] == end[1]))
  {
    return false;
  }

  // check if the player is moving their own piece (or a piece at all)
  if(at(begin)->getPlayer() != playerToMove)
  {
    return false;
  }

  // players can't capture their own pieces
  if(at(end)->getPlayer() == playerToMove)
  {
    return false;
  }

  return PieceLogic::isMoveValid(this,at(begin),begin,end,nullptr);
}

Square* Board::at(const int* coord) const
{
  if(coord[0] < board_width  && coord[0] >= 0 
  && coord[1] < board_height && coord[1] >= 0)
  {
    return board[ coord[0] ][ coord[1] ];
  }
  else
  {
    return nullptr;
  }
}

Square* Board::at(int x,int y) const
{
  if(x < board_width  && x >= 0 
  && y < board_height && y >= 0)
  {
    return board[x][y];
  }
  else
  {
    return nullptr;
  }
}

Player Board::getPlayerToMove() const
{
  return playerToMove;
}

int Board::getTurn() const
{
  return turn;
}
