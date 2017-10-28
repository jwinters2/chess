#include <iostream>

#include "GraphicsManager.h"
#include "Board.h"

int main(int argc, char** argv)
{
  std::string input;
  GraphicsManager::init();
  Board b;

  if(argc > 1 && strcmp(argv[1],"-r") == 0)
  {
    GraphicsManager::setRotateOnBlack(true);
  }

  b.render();
  
  while(!GraphicsManager::programTerminated())
  {
    input = GraphicsManager::getInput();
    b.move(input);
    b.render();
  }

  GraphicsManager::cleanup();
}
