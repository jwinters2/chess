#pragma once 

#include "Enums.h"
class Board;

class Opponent
{
  private:
    const int depth;
    const Player player;

    int minmax(const Board*,Player,int,int,int) const;

    bool isBetterThan(Player,int,int) const;
    bool isBetterThan(Player,const Board*,const Board*) const;

    int scoreLowerBound(Player) const;
    Player nextPlayer(Player) const;

    int badnessThreshold(Player) const;

  public:
    Opponent(int,Player);
    ~Opponent();
    Move getMove(const Board*) const;
};
