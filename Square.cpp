#include "Square.h"

#include <iostream>

Square::Square(Piece pi,Player pl,Color c):
               piece(pi),player(pl),color(c),hasMoved(false),enPassantTurn(-1)
{
}

Square::~Square()
{
}

Piece Square::getPiece() const
{
  return piece;
}

Player Square::getPlayer() const
{
  return player;
}

Color Square::getColor() const
{
  return color;
}

bool Square::getHasMoved() const
{
  return hasMoved;
}

int Square::getEnPassantTurn() const
{
  return enPassantTurn;
}

void Square::setPiece(Piece p)
{
  piece = p;
}

void Square::setPlayer(Player p)
{
  player = p;
}

void Square::setColor(Color c)
{
  color = c;
}

void Square::setEnPassantTurn(int i)
{
  enPassantTurn = i;
}

bool Square::render() const
{
  char c = '-';

  switch(piece)
  {
    case Empty:
      c = ' ';
      break;
    case Pawn:
      c = 'p';
      break;
    case Knight:
      c = 'n';
      break;
    case Bishop:
      c = 'b';
      break;
    case Rook:
      c = 'r';
      break;
    case Queen:
      c = 'q';
      break;
    case King:
      c = 'k';
      break;
  }

  if(player == White)
  {
    c += ('A' - 'a');
  }

  std::cout << c;

  return true;
}

void Square::movePiece(Square* begin,Square* end)
{
  if(begin != nullptr && end != nullptr)
  {
    end->setPiece (begin->getPiece ());
    end->setPlayer(begin->getPlayer());

    begin->setPiece (Empty);
    begin->setPlayer(None);

    end->hasMoved = true;
    begin->hasMoved = false;
  }
}
