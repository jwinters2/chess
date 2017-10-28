#pragma once

#include "Enums.h"

class Square
{
  private:
    Piece piece;
    Player player; 
    Color color;
    bool hasMoved;

  public:
    Square(Piece,Player,Color);
    ~Square();

    Piece getPiece() const;
    Player getPlayer() const;
    Color getColor() const;
    bool getHasMoved() const;

    void setPiece(Piece);
    void setPlayer(Player);
    void setColor(Color);

    bool render() const;

    static void movePiece(Square*,Square*);
};
