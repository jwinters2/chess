#pragma once

#include "Enums.h"

#include <vector>

class Board;
class Square;

class BoardUtils
{
  public:
    static std::vector<int> getBoardScore(const Board*);
    static std::vector<Move> getPossibleMoves(const Board*,Player);

  private:
    BoardUtils();
    ~BoardUtils();

    static int getPieceScore(Piece);
    static int getPieceMoves(const Board*,const Square*);
};
