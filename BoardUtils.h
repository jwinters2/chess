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
    static std::vector<Coord> getPieceMoves(const Board*,int,int);

  private:
    BoardUtils();
    ~BoardUtils();

    static int getPieceScore(Piece);
};
