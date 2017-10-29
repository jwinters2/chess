#pragma once

#include "Enums.h"

#include <string>

class Square;

class GraphicsManager;

class Board
{
  private:
    Square*** board;

    const int board_width;
    const int board_height;

    int turn;

    Player playerToMove;
    int move_begin[2];
    int move_end[2];

    bool parseNotation(std::string,int*,int*);
    void switchPlayer();

  public:
    Board();
    ~Board();

    void render() const;

    bool move(std::string);
    Player getPlayerToMove() const;

    Square* at(const int*) const;
    Square* at(int,int) const;

    int getTurn() const;
};
