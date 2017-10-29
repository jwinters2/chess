#include "BoardUtils.h"

#include "Board.h"
#include "Square.h"
#include "PieceLogic.h"

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
          retval.push_back(Move( x,y , it->x,it->y ));
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
}
