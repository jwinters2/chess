#pragma once

class Board;
class Square;

class PieceLogic
{
  private:
    PieceLogic();

    static bool isPawnMoveValid
                (const Board*,const Square*,const int*,const int*);

    static bool isKnightMoveValid
                (const Board*,const Square*,const int*,const int*);
                
    static bool isBishopMoveValid
                (const Board*,const Square*,const int*,const int*);

    static bool isRookMoveValid
                (const Board*,const Square*,const int*,const int*);

    static bool isQueenMoveValid
                (const Board*,const Square*,const int*,const int*);

    static bool isKingMoveValid
                (const Board*,const Square*,const int*,const int*);
  public:
    static bool isMoveValid
                (const Board*,const Square*,const int*,const int*);
};
