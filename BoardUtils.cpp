#include "BoardUtils.h"

#include "Board.h"
#include "Square.h"
#include "PieceLogic.h"

#include <iostream>

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
      }
      else
      {
        whiteScore -= getPieceScore(board->getPiece(x,y));
      }
    }
  }

  return whiteScore;
}

std::vector<Move> BoardUtils::getPossibleMoves(const Board* board,Player player)
{
  std::vector<Move> retval;
  std::vector<Coord> temp;

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board->getPlayer(x,y) == player)
      {
        temp = getPieceMoves(board,x,y);
        for(auto it = temp.begin(); it != temp.end(); ++it)
        {
          if( (board->getPiece(x,y) == Pawn) &&
          ((it->y == 7 && player == White) || (it->y == 0 && player == Black)))
          {
            retval.push_back( Move( x,y , it->x,it->y , Knight) );
            retval.push_back( Move( x,y , it->x,it->y , Bishop) );
            retval.push_back( Move( x,y , it->x,it->y , Rook) );
            retval.push_back( Move( x,y , it->x,it->y , Queen) );
          }
          else
          {
            retval.push_back(Move( x,y , it->x,it->y ));
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
      return 1;
    case Knight:
    case Bishop:
      return 3;
    case Rook:
      return 5;
    case Queen:
      return 9;
    case King:
      return 10000;
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
  /*
  std::vector<Coord> retval;
  int begin[2] = {s_x,s_y};
  int end[2];

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      end[0] = x;
      end[1] = y;
      if(PieceLogic::isMoveValid(board,begin,end,nullptr))
      {
        retval.push_back(Coord(x,y));
      }
    }
  }

  return retval;
  */
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
                                          enemy,Empty,nullptr,true) )
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
