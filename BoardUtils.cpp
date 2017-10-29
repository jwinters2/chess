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
  Square* s;

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      s = board->at(x,y);
      if(s->getPlayer() == White)
      {
        whiteScore += getPieceScore(s->getPiece());
      }
      else
      {
        blackScore += getPieceScore(s->getPiece());
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
  return std::vector<Move>();
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

std::vector<Coord> BoardUtils::getPieceMoves
                  (const Board* board,const Square* s,int s_x,int s_y)
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
      if(PieceLogic::isMoveValid(board,s,begin,end,nullptr))
      {
        retval.push_back(Coord(x,y));
      }
    }
  }

  return retval;
}
