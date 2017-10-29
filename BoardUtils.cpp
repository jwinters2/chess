#include "BoardUtils.h"

#include "Board.h"
#include "Square.h"

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

int BoardUtils::getPieceMoves(const Board* board,const Square* s)
{
  return 0;
}
