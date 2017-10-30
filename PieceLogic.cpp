#include "PieceLogic.h"

#include "Enums.h"
#include "Square.h"
#include "Board.h"
#include "BoardUtils.h"

bool PieceLogic::isMoveValid
     (const Board* board,const int* begin,const int* end,
      Piece promoteTo,SpecialMove* sm)
{
  return isEitherMoveValid(board,begin,end,
                           board->getPlayerToMove(),promoteTo,sm);
}

bool PieceLogic::isEnemyMoveValid
     (const Board* board,const int* begin,const int* end,
      Piece promoteTo,SpecialMove* sm)
{
  Player enemy = (board->getPlayerToMove() == White ? Black : White);
  return isEitherMoveValid(board,begin,end,enemy,promoteTo,sm);
}

bool PieceLogic::isEitherMoveValid
     (const Board* board,const int* begin,const int* end,
      Player currentPlayer,Piece promoteTo,SpecialMove* sm)
{
  if(sm != nullptr)
  {
    *sm = NoMove;
  }

  // check for valid coordinates
  if( !board->areValidCoordinates(begin[0],begin[1]) 
  || !board->areValidCoordinates(end[0],end[1])
  || (begin[0] == end[0] && begin[1] == end[1]))
  {
    return false;
  }

  // check if the player is moving their own piece (or a piece at all)
  if(board->getPlayer(begin) != currentPlayer)
  {
    return false;
  }

  // players can't capture their own pieces
  if(board->getPlayer(end) == currentPlayer)
  {
    return false;
  }

  SpecialMove tempsm;

  switch(board->getPiece(begin))
  {
    case Pawn:
      if(isPawnMoveValid   (board,begin,end,sm))
      {
        if(end[1] == 7 || end[1] == 0)
        {
          switch(promoteTo)
          {
            case Knight:
              tempsm = PromoteKnight;
              break;
            case Bishop:
              tempsm = PromoteBishop;
              break;
            case Rook:
              tempsm = PromoteRook;
              break;
            case Queen:
              tempsm = PromoteQueen;
              break;
            default:
              return false;
          }

          if(sm != nullptr)
          {
            *sm = tempsm;
          }
        }
        return true;
      }
    case Knight:
      return isKnightMoveValid (board,begin,end);
    case Bishop:
      return isBishopMoveValid (board,begin,end);
    case Rook:
      return isRookMoveValid   (board,begin,end);
    case Queen:
      return isQueenMoveValid  (board,begin,end);
    case King:
      return isKingMoveValid   (board,begin,end,sm);
    default:
      return false;
  }
}

bool PieceLogic::isPawnMoveValid
     (const Board* board,const int* begin,const int* end,SpecialMove* sm)
{
  int direction;
  if(board->getPlayer(begin) == White)
  {
    // white pawns must move up (y coord must increase)
    direction = 1;
  }
  else
  {
    direction = -1;
  }

  // a movement one square forward is valid if unoccuppied
  if(begin[0] == end[0] && begin[1] + direction == end[1])
  {
    if(board->getPiece(begin[0],begin[1]+direction) == Empty)
    {
      return true;
    }
  }

  // pawns can move 2 if they haven't moved before
  if(begin[0] == end[0] && begin[1] + (2*direction) == end[1] 
  && !board->getHasMoved(begin))
  {
    if(board->getPiece(begin[0],begin[1] +    direction)  == Empty
    && board->getPiece(begin[0],begin[1] + (2*direction)) == Empty)
    {
      //board->setEnPassantTurn(end,board->getTurn());
      if(sm != nullptr)
      {
        *sm = MovedEnPassant;
      }
      return true;
    }
  }

  // pawns can move diagonally when capturing (ending square is not empty)
  for(int side = -1; side <= 1; side +=2 )
  {
    if( begin[0] + side == end[0] && begin[1] + direction == end[1] )
    {
      // if our destination isn't empty, we're about to capture a piece
      if(board->getPiece(end) != Empty)
      {
        return true;
      }
      // if it is empty, but there's an en-passant pawn directly to our side
      else if(board->getEnPassantTurn(begin[0] + side,begin[1])
              == board->getTurn() - 1)
      {
        if(sm != nullptr)
        {
          *sm = CapturedEnPassant;
        }
        return true;
      }
    }
  }

  return false;
}

bool PieceLogic::isKnightMoveValid
     (const Board* board,const int* begin,const int* end)
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
     (const Board* board,const int* begin,const int* end)
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
    if(board->getPlayer(cursor) != None)
    {
      return false;
    }

    cursor[0] += dx;
    cursor[1] += dy;
  }

  return true;
}

bool PieceLogic::isRookMoveValid
     (const Board* board,const int* begin,const int* end)
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

  int cursor[2] = {begin[0] + dx,begin[1] + dy};

  while(cursor[0] != end[0] || cursor[1] != end[1])
  {
    // we hit a square occupied by a player, 
    // rooks can't move through other pieces
    if(board->getPlayer(cursor) != None)
    {
      return false;
    }

    cursor[0] += dx;
    cursor[1] += dy;
  }

  return true;
}

bool PieceLogic::isQueenMoveValid
     (const Board* board,const int* begin,const int* end)
{
  return (isBishopMoveValid(board,begin,end) 
       || isRookMoveValid(board,begin,end));
}

bool PieceLogic::isKingMoveValid
     (const Board* board,const int* begin,const int* end,SpecialMove* sm)
{
  for(int dx=-1; dx<=1; dx++)
  {
    for(int dy=-1; dy<=1; dy++)
    {
      // if dx and dy aren't both 0, and end is [dx,dy] from begin, it's valid
      if( (dx != 0 || dy != 0)
        && begin[0] + dx == end[0] && begin[1] + dy == end[1]
        && !BoardUtils::isSquareUnderAttack
            (board,end[0],end[1],board->getPlayer(begin)) )
      {
        return true;
      }
    }
  }

  // check kingside castling
  if(begin[1] == end[1] && begin[0] == 4 && end[0] == 6)
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
      if(board->getPiece(rookPos) == Rook
      && !board->getHasMoved(kingPos) && !board->getHasMoved(rookPos)
      && board->getPiece(5,i) == Empty && board->getPiece(6,i) == Empty
      && !BoardUtils::isSquareUnderAttack(board,4,i,board->getPlayer(begin)) 
      && !BoardUtils::isSquareUnderAttack(board,5,i,board->getPlayer(begin)) 
      && !BoardUtils::isSquareUnderAttack(board,6,i,board->getPlayer(begin)) )
      {
        /*
        Square::movePiece(at(kingPos),at(6,i));
        Square::movePiece(at(rookPos),at(5,i));
        switchPlayer();
        */
        if(sm != nullptr)
        {
          *sm = CastleKingside;
        }

        return true;
      }
    }
  }

  // check queenside castling
  if(begin[1] == end[1] && begin[0] == 4 && end[0] == 2)
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
      if(board->getPiece(rookPos) == Rook
      && !board->getHasMoved(kingPos) && !board->getHasMoved(rookPos)
      && board->getPiece(3,i) == Empty && board->getPiece(2,i) == Empty
      && board->getPiece(1,i) == Empty
      && !BoardUtils::isSquareUnderAttack(board,4,i,board->getPlayer(begin)) 
      && !BoardUtils::isSquareUnderAttack(board,3,i,board->getPlayer(begin)) 
      && !BoardUtils::isSquareUnderAttack(board,2,i,board->getPlayer(begin)) )
      {
        /*
        Square::movePiece(at(kingPos),at(2,i));
        Square::movePiece(at(rookPos),at(3,i));
        switchPlayer();
        */

        if(sm != nullptr)
        {
          *sm = CastleQueenside;
        }

        return true;
      }
    }
  }

  return false;
}
