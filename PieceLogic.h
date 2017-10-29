#pragma once

#include "Enums.h"

class Board;
class Square;

class PieceLogic
{
  private:
    PieceLogic();

    static bool isPawnMoveValid
                (const Board*,const int*,const int*,SpecialMove*);
    static bool isKingMoveValid
                (const Board*,const int*,const int*,SpecialMove*);

    static bool isKnightMoveValid(const Board*,const int*,const int*);
    static bool isBishopMoveValid(const Board*,const int*,const int*);
    static bool isRookMoveValid  (const Board*,const int*,const int*);
    static bool isQueenMoveValid (const Board*,const int*,const int*);
  public:
    static bool isMoveValid
                (const Board*,const int*,const int*,SpecialMove*);
};
