#pragma once

enum Piece { Empty, Pawn, Knight, Bishop, Rook, Queen, King };
enum Player { None, White, Black };
enum Color { WhiteSquare, BlackSquare };

struct Move
{
  int start_x;
  int start_y;
  int end_x;
  int end_y;

  Move(int x1,int y1,int x2,int y2):start_x(x1),start_y(y1),end_x(x2),end_y(y2)
  {
  }
};
