#include "Opponent.h"
#include "Board.h"
#include "BoardUtils.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

// uncomment to allow asserts
//#include <cassert>
//#define NDEBUG

const int Opponent::endSearchDepth = 4;

Opponent::Opponent(int d,Player p):depth(d),player(p)
{
  srand(time(NULL));
}

Opponent::~Opponent()
{
}

int Opponent::minmax(const Board* board,Player currentPlayer,
                    int d,int alpha,int beta) const
{
  if(d <= 0) 
  {
    return BoardUtils::getWhiteScore(board);
  }

  if(BoardUtils::isInCheckmate(board,currentPlayer))
  {
    return scoreLowerBound(currentPlayer);
  }

  std::vector<Move> moves = BoardUtils::getPossibleMoves(board,currentPlayer);
  sortMoves(currentPlayer,moves);

  int score;
  Board tempBoard;
  for(auto it = moves.begin(); it != moves.end(); ++it)
  {
    tempBoard.makeIntoCopyOf(*board);
    tempBoard.move(*it);
    score = minmax(&tempBoard,nextPlayer(currentPlayer),d-1,alpha,beta);
    if(currentPlayer == White)
    {
      // white to move, max
      if(score >= beta)
      {
        return beta;
      }
      if(score > alpha)
      {
        alpha = score;
      }
    }
    else
    {
      // black to move, min
      if(score <= alpha)
      {
        return alpha;
      }
      if(score < beta)
      {
        beta = score;
      }
    }
  }

  if(currentPlayer == White)
  {
    return alpha;
  }
  else
  {
    return beta;
  }

  /*
  if(BoardUtils::isStalemate(board,currentPlayer,moves))
  {
    return 0;
  }

  //std::cout << "minmax, d = " << d << std::endl;

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
      // guarantees that our parent in the search tree won't pick us
      return scoreLowerBound(nextPlayer(currentPlayer));
    }
  }

  return bestScore;
  */
}

Move Opponent::getMove(const Board* board) const
{
  // for each possible move, return the one with the highest score 
  std::vector<Move> moves = BoardUtils::getPossibleMoves(board,player);

  //int currentScore = board->getScore();

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
    current = minmax(&tempBoard,nextPlayer(player),depth,-100000,100000);
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
  for(int i = moves.size()-1; i>=0; i--)
  {
    int j = rand() % (i+1);
    Move buffer = moves[i];
    moves[i] = moves[j];
    moves[j] = buffer;
  }

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
