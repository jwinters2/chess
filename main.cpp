#include <iostream>
#include <cstdlib>

#include "GraphicsManager.h"
#include "Board.h"
#include "Opponent.h"

enum OpponentType {NoOpponent, Human, Computer};

int main(int argc, char** argv)
{
  int opponentDepth = 3;
  bool rob = false;
  OpponentType ot = NoOpponent;

  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i],"-r") == 0)
    {
      rob = true;
    }
    else if(strcmp(argv[i],"-d") == 0)
    {
      if(i+1 < argc)
      {
        opponentDepth = atoi(argv[++i]);
      }
      else
      {
        std::cerr << "invalid use of -d, needs a number (default=3)";;
        std::cerr << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    else if(strcmp(argv[1],"-h") == 0)
    {
      ot = Human;
    }
    else if(strcmp(argv[1],"-c") == 0)
    {
      ot = Computer;
    }
  }

  if(ot == NoOpponent)
  {
      std::cerr << "please specify an opponent with -h (human) ";
      std::cerr << "or -c (computer)" << std::endl;
      exit(EXIT_FAILURE);
  }

  std::string input;
  GraphicsManager::init();
  GraphicsManager::setRotateOnBlack(rob);
  Board b;
  Opponent o(opponentDepth,Black);

  b.render();
  
  while(!GraphicsManager::programTerminated())
  {
    if(b.getPlayerToMove() == White || ot == Human)
    {
      input = GraphicsManager::getInput();
      b.move(input);
    }
    else
    {
      b.move(o.getMove(&b));
    }

    b.render();
  }

  GraphicsManager::cleanup();
}
