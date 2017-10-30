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
                    int d,int alphabeta,int original) const
{
  int score = board->getScore();

  if(d <= 0 || score > 9000 || score < -9000)
  {
    return score;
  }

  //std::cout << "minmax, d = " << d << std::endl;

  std::vector<Move> moves = BoardUtils::getPossibleMoves(board,currentPlayer);

  int bestScore = scoreLowerBound(currentPlayer);

  if(moves.empty())
  {
    return bestScore;
  }

  Board tempBoard;
  int current;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    current = minmax(&tempBoard,nextPlayer(currentPlayer),
                      d-1,bestScore,original);

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

  int currentScore = board->getScore();

  Move bestMove;
  int bestScore = scoreLowerBound(player);
  Board tempBoard;
  if(moves.empty())
  {
    std::cerr << "opponent has no moves to check" << std::endl;
    return Move();
  }

  int current;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    current = minmax(&tempBoard,nextPlayer(player),
                     depth,bestScore,currentScore);
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
  return isBetterThan(p,a->getScore(),b->getScore());
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
