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

  private:
    BoardUtils();
    ~BoardUtils();

    static std::vector<Coord> getPawnMoves  (const Board*,int,int);
    static std::vector<Coord> getKnightMoves(const Board*,int,int);
    static std::vector<Coord> getBishopMoves(const Board*,int,int);
    static std::vector<Coord> getRookMoves  (const Board*,int,int);
    static std::vector<Coord> getQueenMoves (const Board*,int,int);
    static std::vector<Coord> getKingMoves  (const Board*,int,int);
};
