#pragma once

enum Piece { Empty='e', Pawn='p', Knight='n', Bishop='b', Rook='r', 
             Queen='q', King='k' };
enum Player { None, White, Black };
enum Color { WhiteSquare, BlackSquare };
enum SpecialMove { NoMove, MovedEnPassant, CapturedEnPassant, 
                   CastleKingside, CastleQueenside,
                   PromoteKnight, PromoteBishop, PromoteRook, PromoteQueen};

struct Move
{
  int start_x;
  int start_y;
  int end_x;
  int end_y;
  Piece promote;

  Move():start_x(0),start_y(0),end_x(0),end_y(0),promote(Empty)
  {
  }

  Move(int x1,int y1,int x2,int y2,Piece pi):start_x(x1),start_y(y1),
                                            end_x(x2),end_y(y2),promote(pi)
  {
  }

  Move(int x1,int y1,int x2,int y2):start_x(x1),start_y(y1),
                                    end_x(x2),end_y(y2),promote(Empty)
  {
  }

  Move(const Move& o):start_x(o.start_x),start_y(o.start_y),
                      end_x(o.end_x),end_y(o.end_y),promote(o.promote)
  {
  }
};

struct Coord
{
  int x;
  int y;

  Coord():x(0),y(0)
  {
  }

  Coord(int x1,int y1):x(x1),y(y1)
  {
  }

  Coord(const Coord& o):x(o.x),y(o.y)
  {
  }
};
