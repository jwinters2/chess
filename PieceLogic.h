#pragma once

#include "Enums.h"

class Board;
class Square;

class PieceLogic
{
  private:
    PieceLogic();

    static bool isPawnMoveValid
                (const Board*,const int*,const int*,SpecialMove*,bool=false);
    static bool isKingMoveValid
                (const Board*,const int*,const int*,SpecialMove*,bool=false);

    static bool isKnightMoveValid
                (const Board*,const int*,const int*,bool=false);
    static bool isBishopMoveValid
                (const Board*,const int*,const int*,bool=false);
    static bool isRookMoveValid
                (const Board*,const int*,const int*,bool=false);
    static bool isQueenMoveValid
                (const Board*,const int*,const int*,bool=false);
  public:
    static bool isMoveValid
                (const Board*,const int*,const int*,
                 Piece,SpecialMove*,bool=false);
    static bool isEnemyMoveValid
                (const Board*,const int*,const int*,
                 Piece,SpecialMove*,bool=false);
    static bool isEitherMoveValid
                (const Board*,const int*,const int*,
                 Player,Piece,SpecialMove*,bool=false);
    static bool isPawnAttacking(const Board*,const int*,const int*,Player);
};
