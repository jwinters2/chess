#include <iostream>
#include <fstream>
#include <cstdlib>

#include "GraphicsManager.h"
#include "Board.h"
#include "Opponent.h"

int main(int argc, char** argv)
{
  int opponentDepth = 3;
  bool rob = false;
  bool computerOp = true;
  bool readFromFile = false;
  std::string filename = "";

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
        std::cerr << "invalid use of -d, needs a number (default=3)";
        std::cerr << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    else if(strcmp(argv[i],"-h") == 0)
    {
       computerOp = false;
    }
    else if(strcmp(argv[i],"-o") == 0)
    {
      readFromFile = true;
      if(i+1 < argc)
      {
        std::string buffer(argv[++i]);
        filename = buffer;
      }
      else
      {
        std::cerr << "invalid use of -o, needs a filename" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }

  std::string input;
  GraphicsManager::init();
  GraphicsManager::setRotateOnBlack(rob);
  Board b;
  Opponent o(opponentDepth,Black);

  if(readFromFile)
  {
    std::ifstream file;
    std::cout << "opening " << filename << std::endl;
    file.open("boardstates/" + filename + ".bs");
    if(file.is_open())
    {
      b.setupFromFile(file);
    }
  }

  b.render();
  
  while(!GraphicsManager::programTerminated() && !b.hasEnded())
  {
    if(b.getPlayerToMove() == White || !computerOp)
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
