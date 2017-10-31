#include "Opponent.h"
#include "Board.h"
#include "BoardUtils.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
//#include <cassert>

// uncomment to allow asserts
//#define NDEBUG

const int Opponent::endSearchDepth = 2;

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
  int score = BoardUtils::getWhiteScore(board);
  
  /*if(score != BoardUtils::getWhiteScore(board))
  {
    std::cout << *board << std::endl;
    std::cout << "board->getScore()           = " << score << std::endl;
    std::cout << "BoardUtils::getWhiteScore() = ";
    std::cout << BoardUtils::getWhiteScore(board) << std::endl;
    assert(score == BoardUtils::getWhiteScore(board));
  }*/

  if(d <= 0 || BoardUtils::isInCheckmate(board,currentPlayer))
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

  sortMoves(player,moves);

  // the deeper we are, the more of the worst ones we ignore
  moves.resize((moves.size() * (d + endSearchDepth)) 
               / (depth + endSearchDepth));

  Board tempBoard;
  int current;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    current = minmax(&tempBoard,nextPlayer(currentPlayer),d-1,bestScore);

    if(isBetterThan(currentPlayer,current,bestScore))
    {
      bestScore = current;
    }

    if(isBetterThanEqualTo(currentPlayer,bestScore,alphabeta))
    {
      return bestScore;
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
  
  sortMoves(player,moves);

  int current;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    current = minmax(&tempBoard,nextPlayer(player),depth,bestScore);
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

bool Opponent::isBetterThanEqualTo(Player p,int a,int b) const
{
  if(p == White)
  {
    // the white player wants the highest score possible
    return (a >= b);
  }
  else
  {
    return (a <= b);
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

void Opponent::sortMoves(Player player,std::vector<Move>& moves) const
{
  if(player == White)
  {
    std::sort(moves.begin(),moves.end(),compareMovesWhite);
  }
  else
  {
    std::sort(moves.begin(),moves.end(),compareMovesBlack);
  }
}

bool Opponent::compareMovesWhite(const Move& a,const Move& b)
{
  return a.score > b.score;
}

bool Opponent::compareMovesBlack(const Move& a,const Move& b)
{
  return a.score < b.score;
}
