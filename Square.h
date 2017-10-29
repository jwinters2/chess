#pragma once

#include "Enums.h"

class Square
{
  private:
    Piece piece;
    Player player; 
    Color color;
    bool hasMoved;
    int enPassantTurn;

  public:
    Square();
    Square(Piece,Player,Color);
    ~Square();

    Piece getPiece() const;
    Player getPlayer() const;
    Color getColor() const;
    bool getHasMoved() const;
    int getEnPassantTurn() const;

    void setPiece(Piece);
    void setPlayer(Player);
    void setColor(Color);
    void setEnPassantTurn(int);

    bool render() const;

    static void movePiece(Square&,Square&);

    void makeIntoCopyOf(const Square&);
};
