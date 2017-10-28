#include "PieceLogic.h"

#include "Enums.h"
#include "Square.h"
#include "Board.h"

#include <iostream>

bool PieceLogic::isMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  if(s == nullptr)
  {
    return false;
  }

  switch(s->getPiece())
  {
    case Pawn:
      return isPawnMoveValid   (board,s,begin,end);
    case Knight:
      return isKnightMoveValid (board,s,begin,end);
    case Bishop:
      return isBishopMoveValid (board,s,begin,end);
    case Rook:
      return isRookMoveValid   (board,s,begin,end);
    case Queen:
      return isQueenMoveValid  (board,s,begin,end);
    case King:
      return isKingMoveValid   (board,s,begin,end);
    default:
      return false;
  }
}

bool PieceLogic::isPawnMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  int direction;
  if(s->getPlayer() == White)
  {
    // white pawns must move up (y coord must increase)
    direction = 1;
  }
  else
  {
    direction = -1;
  }

  // a movement one square forward is valid
  if(begin[0] == end[0] && begin[1] + direction == end[1])
  {
    return true;
  }

  // pawns can move 2 if they haven't moved before
  if(begin[0] == end[0] && begin[1] + (2*direction) == end[1] 
  && !s->getHasMoved())
  {
    return true;
  }

  // pawns can move diagonally when capturing (ending square is not empty)
  if( (begin[0] + 1 == end[0] || begin[0] - 1 == end[0] )
  && begin[1] + direction == end[1] && board->at(end)->getPiece() != Empty)
  {
    return true;
  }

  return false;
}

bool PieceLogic::isKnightMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  for(int dx = -1; dx <= 1; dx += 2)
  {
    for(int dy = -1; dy <= 1; dy += 2)
    {
      if( (begin[0] +    dx  == end[0] && begin[1] + (2*dy) == end[1])
      ||  (begin[0] + (2*dx) == end[0] && begin[1] +    dy  == end[1]) )
      {
        return true;
      }
    }
  }

  return false;
}
                
bool PieceLogic::isBishopMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  // bishop moves must be diagonal (dx == dy or dx == -dy)
  if(begin[0] - end[0] != begin[1] - end[1]
  && begin[0] - end[0] != end[1] - begin[1])
  {
    return false;
  }

  // figure out which direction it's going (in coordinates)
  int dx = (end[0]>begin[0]?1:-1);
  int dy = (end[1]>begin[1]?1:-1);

  int cursor[2] = {begin[0] + dx,begin[1] + dy};

  while(cursor[0] != end[0] || cursor[1] != end[1])
  {
    // we hit a square occupied by a player, 
    // bishops can't move through other pieces
    if(board->at(cursor)->getPlayer() != None)
    {
      return false;
    }

    cursor[0] += dx;
    cursor[1] += dy;
  }

  return true;
}

bool PieceLogic::isRookMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  // rook moves must be orthogonal (dx == 0 or dy == 0)
  if(begin[0] != end[0] && begin[1] != end[1])
  {
    return false;
  }

  // figure out which direction it's going (in coordinates)
  int dx = (end[0]>begin[0]?1:-1);
  int dy = (end[1]>begin[1]?1:-1);

  if(begin[0] == end[0])
  {
    dx = 0;
  }

  if(begin[1] == end[1])
  {
    dy = 0;
  }

  std::cerr << "[dx,dy] = [" << dx << "," << dy << "]" << std::endl;

  int cursor[2] = {begin[0] + dx,begin[1] + dy};

  while(cursor[0] != end[0] || cursor[1] != end[1])
  {
    // we hit a square occupied by a player, 
    // rooks can't move through other pieces
    if(board->at(cursor)->getPlayer() != None)
    {
      return false;
    }

    cursor[0] += dx;
    cursor[1] += dy;
  }

  return true;
}

bool PieceLogic::isQueenMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  return (isBishopMoveValid(board,s,begin,end) 
       || isRookMoveValid(board,s,begin,end));
}

bool PieceLogic::isKingMoveValid
     (const Board* board,const Square* s,const int* begin,const int* end)
{
  for(int dx=-1; dx<=1; dx++)
  {
    for(int dy=-1; dy<=1; dy++)
    {
      // if dx and dy aren't both 0, and end is [dx,dy] from begin, it's valid
      if( (dx != 0 || dy != 0)
        && begin[0] + dx == end[0] && begin[1] + dy == end[1])
      {
        return true;
      }
    }
  }

  return false;
}
