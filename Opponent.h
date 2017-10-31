#pragma once 

#include "Enums.h"
#include <vector>
class Board;

class Opponent
{
  private:
    const int depth;
    const Player player;

    static const int endSearchDepth;

    int minmax(const Board*,Player,int,int) const;

    bool isBetterThan(Player,int,int) const;
    bool isBetterThanEqualTo(Player,int,int) const;
    bool isBetterThan(Player,const Board*,const Board*) const;

    int scoreLowerBound(Player) const;
    Player nextPlayer(Player) const;

    int badnessThreshold(Player) const;

    void sortMoves(Player,std::vector<Move>&) const;
    static bool compareMovesWhite(const Move&,const Move&);
    static bool compareMovesBlack(const Move&,const Move&);

  public:
    Opponent(int,Player);
    ~Opponent();
    Move getMove(const Board*) const;
};
