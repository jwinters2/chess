#pragma once

#include "Enums.h"

#include <vector>

class Board;
class Square;

class BoardUtils
{
  public:
    static std::vector<int> getBoardScore(const Board*);
    static int getWhiteScore(const Board* board);
    static std::vector<Move> getPossibleMoves(const Board*,Player);
    static std::vector<Coord> getPieceMoves(const Board*,int,int);

    static int getPieceScore(Piece);
    
    static bool isSquareUnderAttack(const Board*,int,int,Player);
    static bool isInCheck(const Board*,Player);
    static bool isInCheckmate(const Board*,Player);
    static bool isBareKings(const Board*);

  private:
    BoardUtils();
    ~BoardUtils();

    static const int attackingValue;
    static const int defendingValue;

    static std::vector<Coord> getPawnMoves  (const Board*,int,int);
    static std::vector<Coord> getKnightMoves(const Board*,int,int);
    static std::vector<Coord> getBishopMoves(const Board*,int,int);
    static std::vector<Coord> getRookMoves  (const Board*,int,int);
    static std::vector<Coord> getQueenMoves (const Board*,int,int);
    static std::vector<Coord> getKingMoves  (const Board*,int,int);

    static int countPieceMoves (const Board*,int,int);
    
    static int countPawnMoves  (const Board*,int,int);
    static int countKnightMoves(const Board*,int,int);
    static int countBishopMoves(const Board*,int,int);
    static int countRookMoves  (const Board*,int,int);
    static int countQueenMoves (const Board*,int,int);
    static int countKingMoves  (const Board*,int,int);

    static int movementValue   (const Board*,int,int,Player);
};
