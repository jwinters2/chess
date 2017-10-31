#include "Enums.h"

std::ostream& operator<<(std::ostream& out,const Move& m)
{
  out << "(" << m.start_x << "," << m.start_y << ") -> (";
  out << m.end_x << "," << m.end_y << ") p=" << (char)(m.promote);
  return out;
}

std::ostream& operator<<(std::ostream& out,const Coord& c)
{
  out << "(" << c.x << "," << c.y << ")";
  return out;
}
