#include "Opponent.h"
#include "Board.h"
#include "BoardUtils.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

Opponent::Opponent(int d,Player p):depth(d),player(p)
{
  srand(time(NULL));
}

Opponent::~Opponent()
{
}

int Opponent::minmax(const Board* board,Player currentPlayer,
                    int d,int alphabeta) const
{
  if(d <= 0)
  {
    return BoardUtils::getWhiteScore(board);
  }
  
  //std::cout << "minmax, d = " << d << std::endl;

  std::vector<Move> moves = BoardUtils::getPossibleMoves(board,currentPlayer);

  int bestScore = scoreLowerBound(currentPlayer);
  Board tempBoard;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    int current = minmax(&tempBoard,nextPlayer(currentPlayer),d-1,bestScore);

    if(isBetterThan(currentPlayer,current,bestScore))
    {
      bestScore = current;

      if(isBetterThan(currentPlayer,bestScore,alphabeta))
      {
        return bestScore;
      }
    }
  }

  return bestScore;
}

Move Opponent::getMove(const Board* board) const
{
  // for each possible move, return the one with the highest score 
  std::vector<Move> moves = BoardUtils::getPossibleMoves(board,player);

  Move bestMove;
  int bestScore = scoreLowerBound(player);
  Board tempBoard;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    int current = minmax(&tempBoard,nextPlayer(player),depth,bestScore);
    if(isBetterThan(player,current,bestScore))
    {
      bestScore = current;
      bestMove = *it;
    }
  }

  return bestMove;
}

bool Opponent::isBetterThan(Player p,const Board* a,const Board* b) const
{
  return isBetterThan(p,
                      BoardUtils::getWhiteScore(a),
                      BoardUtils::getWhiteScore(b));
}

bool Opponent::isBetterThan(Player p,int a,int b) const
{
  if(a == b)
  {
    return (rand()%2 == 0);
  }
  else if(p == White)
  {
    // the white player wants the highest score possible
    return (a > b);
  }
  else
  {
    return (a < b);
  }
}

int Opponent::scoreLowerBound(Player p) const
{
  return (p == White ? -20000 : 20000);
}

int Opponent::badnessThreshold(Player p) const
{
  return (p == White ? -3 : 3);
}

Player Opponent::nextPlayer(Player p) const
{
  if(p == White)
  {
    return Black;
  }
  else
  {
    return White;
  }
}
