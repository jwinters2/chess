#pragma once

#include "Enums.h"
#include "Square.h"

#include <string>

class GraphicsManager;

class Board
{
  friend std::ostream& operator<<(std::ostream&,const Board&);

  private:
    Square board[8][8];

    const int board_width;
    const int board_height;

    int turn;
    int score;
    void addScore(int);

    Player playerToMove;
    int move_begin[2];
    int move_end[2];

    void switchPlayer();

    Square& at(const int*);
    Square& at(int,int);

    void clearBoard();

  public:
    Board();
    Board(const Board&);
    ~Board();
    void makeIntoCopyOf(const Board&);

    void render() const;

    bool move(std::string);
    bool move(const Move&,bool=false);
    void performSpecialMove(const int*,const int*,SpecialMove);
    bool parseNotation(std::string,int*,int*,SpecialMove*) const;
    Player getPlayerToMove() const;

    Piece getPiece(int,int) const;
    Player getPlayer(int,int) const;
    Color getColor(int,int) const;
    bool getHasMoved(int,int) const;
    int getEnPassantTurn(int,int) const;

    void setPiece(int,int,Piece);
    void setPlayer(int,int,Player);
    void setColor(int,int,Color);
    void setEnPassantTurn(int,int,int);

    Piece getPiece(const int*) const;
    Player getPlayer(const int*) const;
    Color getColor(const int*) const;
    bool getHasMoved(const int*) const;
    int getEnPassantTurn(const int*) const;

    void setPiece(const int*,Piece);
    void setPlayer(const int*,Player);
    void setColor(const int*,Color);
    void setEnPassantTurn(const int*,int);

    bool areValidCoordinates(int,int) const;

    int getTurn() const;
    int getScore() const;

    void setupDefault();
    void setupFromFile(std::ifstream&);

    bool hasEnded() const;
};
