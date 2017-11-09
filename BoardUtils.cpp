#include "BoardUtils.h"

#include "Board.h"
#include "Square.h"
#include "PieceLogic.h"

#include <iostream>

const int BoardUtils::attackingValue = 4;
const int BoardUtils::defendingValue = 2;

std::vector<int> BoardUtils::getBoardScore(const Board* board)
{
  if(board == nullptr)
  {
    return std::vector<int>();
  }

  int whiteScore = 0;  
  int blackScore = 0;  

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == White)
      {
        whiteScore += getPieceScore(board->getPiece(x,y));
      }
      else
      {
        blackScore += getPieceScore(board->getPiece(x,y));
      }
    }
  }

  std::vector<int> retval;
  retval.push_back(whiteScore);
  retval.push_back(blackScore);

  return retval;
}

int BoardUtils::getWhiteScore(const Board* board)
{
  if(board == nullptr)
  {
    return 0;
  }

  int whiteScore = 0;  

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == White)
      {
        whiteScore += getPieceScore(board->getPiece(x,y));
        whiteScore += countPieceMoves(board,x,y);
      }
      else
      {
        whiteScore -= getPieceScore(board->getPiece(x,y));
        whiteScore -= countPieceMoves(board,x,y);
      }
    }
  }

  return whiteScore;
}

std::vector<Move> BoardUtils::getPossibleMoves(const Board* board,Player player)
{
  std::vector<Move> retval;
  std::vector<Coord> temp;
  int deltaScore;

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == player)
      {
        temp = getPieceMoves(board,x,y);
        for(auto it = temp.begin(); it != temp.end(); ++it)
        {
          deltaScore = (player == White ? 1 : -1);
          deltaScore *= BoardUtils::getPieceScore(board->getPiece(it->x,it->y));

          if( (board->getPiece(x,y) == Pawn) &&
          ((it->y == 7 && player == White) || (it->y == 0 && player == Black)))
          {
            retval.push_back( Move( x,y , it->x,it->y , Knight, deltaScore) );
            retval.push_back( Move( x,y , it->x,it->y , Bishop, deltaScore) );
            retval.push_back( Move( x,y , it->x,it->y , Rook,   deltaScore) );
            retval.push_back( Move( x,y , it->x,it->y , Queen,  deltaScore) );
          }
          else
          {
            retval.push_back( Move( x,y , it->x,it->y , Empty,  deltaScore) );
          }
        }
      }
    }
  }

  return retval;
}

int BoardUtils::getPieceScore(Piece piece)
{
  switch(piece)
  {
    case Empty:
      return 0;
    case Pawn:
      return 200;
    case Knight:
      return 600;
    case Bishop:
      return 650;
    case Rook:
      return 1000;
    case Queen:
      return 1800;
    case King:
      return 2000000;
  }

  return 0;
}

std::vector<Coord> BoardUtils::getPieceMoves(const Board* board,int s_x,int s_y)
{
  switch(board->getPiece(s_x,s_y))
  {
    case Pawn:
      return getPawnMoves(board,s_x,s_y);
    case Knight:
      return getKnightMoves(board,s_x,s_y);
    case Bishop:
      return getBishopMoves(board,s_x,s_y);
    case Rook:
      return getRookMoves(board,s_x,s_y);
    case Queen:
      return getQueenMoves(board,s_x,s_y);
    case King:
      return getKingMoves(board,s_x,s_y);
    default:
      return std::vector<Coord>();
  }
}

std::vector<Coord> BoardUtils::getPawnMoves(const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  int direction = (board->getPlayer(s_x,s_y) == White ? 1 : -1);

  // pawns can possibly move one square forward
  end[0] = begin[0];
  end[1] = begin[1] + direction;
  if(PieceLogic::isMoveValid(board,begin,end,Queen,nullptr))
  {
    retval.push_back(Coord(end[0],end[1]));
  }

  // pawns can possibly move two squares forward
  end[0] = begin[0];
  end[1] = begin[1] + (2 * direction);
  if(PieceLogic::isMoveValid(board,begin,end,Queen,nullptr))
  {
    retval.push_back(Coord(end[0],end[1]));
  }
  
  // pawns can possibly move one square diagonally forward
  end[0] = begin[0] + 1;
  end[1] = begin[1] + direction;
  if(PieceLogic::isMoveValid(board,begin,end,Queen,nullptr))
  {
    retval.push_back(Coord(end[0],end[1]));
  }

  end[0] = begin[0] -1;
  end[1] = begin[1] + direction;
  if(PieceLogic::isMoveValid(board,begin,end,Queen,nullptr))
  {
    retval.push_back(Coord(end[0],end[1]));
  }

  return retval;
}

std::vector<Coord> BoardUtils::getKnightMoves
                   (const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  for(int dx = -1; dx <= 1; dx += 2)
  {
    for(int dy = -1; dy <= 1; dy += 2)
    {
      end[0] = begin[0] + dx;
      end[1] = begin[1] + (2*dy);
      if(PieceLogic::isMoveValid(board,begin,end,Empty,nullptr))
      {
        retval.push_back(Coord(end[0],end[1]));
      }

      end[0] = begin[0] + (2*dx);
      end[1] = begin[1] + dy;
      if(PieceLogic::isMoveValid(board,begin,end,Empty,nullptr))
      {
        retval.push_back(Coord(end[0],end[1]));
      }
    }
  }

  return retval;
}
std::vector<Coord> BoardUtils::getBishopMoves
                   (const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  int i;
  bool sucessfull;

  for(int dx = -1; dx <= 1; dx += 2)
  {
    for(int dy = -1; dy <= 1; dy += 2)
    {
      // walk along the direction until we fail
      i = 1;
      sucessfull = true;

      while(sucessfull)
      {
        end[0] = begin[0] + (i*dx);
        end[1] = begin[1] + (i*dy);

        sucessfull = PieceLogic::isMoveValid(board,begin,end,Empty,nullptr);
        if(sucessfull)
        {
          retval.push_back(Coord(end[0],end[1]));
        }
        
        i++;
      }
    }
  }

  return retval;
}

std::vector<Coord> BoardUtils::getRookMoves(const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  int i;
  bool sucessfull;

  for(int dx = -1; dx <= 1; dx += 2)
  {
    // walk along the direction until we fail
    i = 1;
    sucessfull = true;

    while(sucessfull)
    {
      end[0] = begin[0] + (i*dx);
      end[1] = begin[1];

      sucessfull = PieceLogic::isMoveValid(board,begin,end,Empty,nullptr);
      if(sucessfull)
      {
        retval.push_back(Coord(end[0],end[1]));
      }
        
      i++;
    }
  }

  for(int dy = -1; dy <= 1; dy += 2)
  {
    // walk along the direction until we fail
    i = 1;
    sucessfull = true;

    while(sucessfull)
    {
      end[0] = begin[0];
      end[1] = begin[1] + (i*dy);

      sucessfull = PieceLogic::isMoveValid(board,begin,end,Empty,nullptr);
      if(sucessfull)
      {
        retval.push_back(Coord(end[0],end[1]));
      }
        
      i++;
    }
  }

  return retval;
}
 
std::vector<Coord> BoardUtils::getQueenMoves(const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval = getRookMoves(board,s_x,s_y);
  std::vector<Coord> bishopMoves = getBishopMoves(board,s_x,s_y);
  
  retval.insert(retval.end(),bishopMoves.begin(),bishopMoves.end());

  return retval;
}

std::vector<Coord> BoardUtils::getKingMoves(const Board* board,int s_x,int s_y)
{
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  for(int dx = -1; dx <= 1; dx++)
  {
    for(int dy = -1; dy <= 1; dy++)
    {
      if(dx != 0 || dy != 0)
      {
        end[0] = begin[0] + dx;
        end[1] = begin[1] + dy;
        if(PieceLogic::isMoveValid(board,begin,end,Empty,nullptr))
        {
          retval.push_back(Coord(end[0],end[1]));
        }
      }
    }
  }

  // check for both sides of castling
  for(int dx = -2; dx <= 2; dx += 4)
  {
    end[0] = begin[0] + dx;
    end[1] = begin[1];
    if(PieceLogic::isMoveValid(board,begin,end,Empty,nullptr))
    {
      retval.push_back(Coord(end[0],end[1]));
    }
  }

  return retval;
}

bool BoardUtils::isSquareUnderAttack(const Board* board,
                                     int s_x,int s_y,Player player)
{
  if(!board->areValidCoordinates(s_x,s_y))
  {
    return false;
  }

  Player enemy = (player == White ? Black : White);

  int begin[2];
  int end[2] = {s_x,s_y};

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == enemy)
      {
        begin[0] = x;
        begin[1] = y;
        if( PieceLogic::isEitherMoveValid(board,begin,end,
                                          enemy,Empty,nullptr,true) 
        && (board->getPiece(begin) != Pawn
        || PieceLogic::isPawnAttacking(board,begin,end,enemy)) )
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool BoardUtils::isInCheck(const Board* board,Player player)
{
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == player && board->getPiece(x,y) == King)
      {
        //std::cout << x << "," << y << "  " << (player == White) << std::endl;
        return(BoardUtils::isSquareUnderAttack(board,x,y,player));
      }
    }
  }

  return false;
}

bool BoardUtils::isInCheckmate(const Board* board,Player player)
{
  if(!isInCheck(board,player))
  {
    //std::cout << "isInCheckmate = false (1)" << std::endl;
    return false;
  }

  auto moves = BoardUtils::getPossibleMoves(board,player);
  Board tempBoard;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);

    if(!isInCheck(&tempBoard,player))
    {
      //std::cout << tempBoard;
      return false;
    }
  }

  return true;
}

bool BoardUtils::isBareKings(const Board* board)
{
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPiece(x,y) != Empty && board->getPiece(x,y) != King)
      {
        return false;
      }
    }
  }

  return true;
}

bool BoardUtils::isStalemate(const Board* board,Player player,
                             const std::vector<Move>& moves)
{
  if(isBareKings(board))
  {
    return true;
  }

  return (moves.empty() && !isInCheck(board,player));
}

int BoardUtils::countPieceMoves (const Board* board,int s_x,int s_y)
{
  switch(board->getPiece(s_x,s_y))
  {
    case Pawn:
      return countPawnMoves(board,s_x,s_y);
    case Knight:
      return countKnightMoves(board,s_x,s_y);
    case Bishop:
      return countBishopMoves(board,s_x,s_y);
    case Rook:
      return countRookMoves(board,s_x,s_y);
    case Queen:
      return countQueenMoves(board,s_x,s_y);
    case King:
      return countKingMoves(board,s_x,s_y);
    default:
      return 0;
  }
}

int BoardUtils::countPawnMoves  (const Board* board,int s_x,int s_y)
{
  int retval = 0;
  Player p = board->getPlayer(s_x,s_y);
  int direction = (p == White ? 1 : -1);

  retval += movementValue(board,s_x,s_y + direction,p);
  if(retval >= attackingValue)
  {
    retval -= attackingValue;
  }

  if(retval != 0)
  {
    retval += movementValue(board,s_x,s_y + (2*direction) ,p);
    if(retval > attackingValue)
    {
      retval -= attackingValue;
    }
  }

  if(movementValue(board,s_x + 1,s_y + direction,p) == attackingValue)
  {
    retval += attackingValue; 
  }
  if(movementValue(board,s_x - 1,s_y + direction,p) == attackingValue)
  {
    retval += attackingValue; 
  }

  return retval;
}

int BoardUtils::countKnightMoves(const Board* board,int s_x,int s_y)
{
  int retval = 0;
  Player p = board->getPlayer(s_x,s_y);

  for(int dx = -1; dx <= 1; dx+=2)
  {
    for(int dy = -1; dy <= 1; dy+=2)
    {
      retval += movementValue(board,s_x + (dx*2),s_y +  dy   ,p);
      retval += movementValue(board,s_x +  dx   ,s_y + (dy*2),p);
    }
  }

  return retval;
}

int BoardUtils::countBishopMoves(const Board* board,int s_x,int s_y)
{
  int retval = 0;
  int i;
  int result = 1;
  Player p = board->getPlayer(s_x,s_y);

  for(int dx = -1; dx <= 1; dx+=2)
  {
    for(int dy = -1; dy <= 1; dy+=2)
    {
      i = 1;
      while(result == 1)
      {
        result = movementValue(board,s_x + (i*dx),s_y + (i*dy),p);
        retval += result;
        i++;
      }
    }
  }

  return result;
}

int BoardUtils::countRookMoves  (const Board* board,int s_x,int s_y)
{
  int retval = 0;
  int i;
  int result = 1;
  Player p = board->getPlayer(s_x,s_y);

  for(int dx = -1; dx <= 1; dx+=2)
  {
    i = 1;
    while(result == 1)
    {
      result = movementValue(board,s_x + (i*dx),s_y,p);
      retval += result;
      i++;
    }
  }

  for(int dy = -1; dy <= 1; dy+=2)
  {
    i = 1;
    while(result == 1)
    {
      result = movementValue(board,s_x,s_y + (i*dy),p);
      retval += result;
      i++;
    }
  }

  return result;
}

int BoardUtils::countQueenMoves (const Board* board,int s_x,int s_y)
{
  return countRookMoves(board,s_x,s_y) + countBishopMoves(board,s_x,s_y);
}

int BoardUtils::countKingMoves  (const Board* board,int s_x,int s_y)
{
  int retval = 0;
  Player p = board->getPlayer(s_x,s_y);

  for(int dx = -1; dx <=1; dx++)
  {
    for(int dy = -1; dy <=1; dy++)
    {
      if(dx == 0 && dy == 0)
      {
        continue;
      }

      if(!isSquareUnderAttack(board,s_x + dx,s_y + dy,p))
      {
        retval++;
      }
    }
  }
  
  // TODO: give a reward for being able to do (or having done) castling

  return retval;
}

int BoardUtils::movementValue   (const Board* board,int s_x,int s_y,Player p)
{
  if(s_x < 0 || s_x >= 8 || s_y < 0 || s_x >= 8)
  {
    return 0;
  }

  Player toAttack = board->getPlayer(s_x,s_y);
  if(toAttack == None)
  {
    return 1;
  }
  return (p == toAttack ? defendingValue : attackingValue);
}
