#include "GraphicsManager.h"

#include "Board.h"
#include "Square.h"
#include "Enums.h"
#include "BoardUtils.h"

#include <iostream>
#include <cstdlib>
#include <vector>

const Board* GraphicsManager::lastBoard = nullptr;
bool GraphicsManager::childShouldTerminate = false;

pthread_mutex_t GraphicsManager::render_mutex;
pthread_mutex_t GraphicsManager::event_mutex;
pthread_t GraphicsManager::render_tid;
pthread_t GraphicsManager::event_tid;
std::string GraphicsManager::inputBuffer = "";
bool GraphicsManager::timeToReadInput = false;
bool GraphicsManager::shouldQuit = false;

const int GraphicsManager::square_size = 80;
const int GraphicsManager::border_size = 12;
const int GraphicsManager::textbox_size = 48;
const int GraphicsManager::text_width = 22;
const int GraphicsManager::text_height = 38;
const int GraphicsManager::text_spacing = 18;

bool GraphicsManager::gameOver = false;
std::string GraphicsManager::gameOverText = "";

SDL_Window* GraphicsManager::window;
SDL_Renderer* GraphicsManager::renderer;
SDL_Texture** GraphicsManager::tex;
    
bool GraphicsManager::rotateOnBlack = false;

bool GraphicsManager::init()
{
  // initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    std::cerr << "Failed to init SDL: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }

  int window_width  = (8*square_size) + (3*border_size);
  int window_height = (8*square_size) + (3*border_size) + textbox_size;
  //std::cout << "Creating window (" << window_width << ",";
  //std::cout << window_height << ")" << std::endl;

  // create a window
  window = SDL_CreateWindow("Chess by James",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            window_width,
                            window_height,
                            SDL_WINDOW_SHOWN);
  if(window == nullptr)
  {
    std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  // create a renderer
  renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
  if(renderer == nullptr)
  {
    std::cerr << "Failed to create SDL renderer: ";
    std::cerr << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  // setup tex array
  tex = new SDL_Texture*[TEXTURE_COUNT];
  for(int i=0; i<TEXTURE_COUNT; i++)
  {
    tex[i] = nullptr;
  }

  // load textures
  loadTexture(background,"background");
  loadTexture(text,"text");

  loadTexture(white_square,"white_square");
  loadTexture(black_square,"black_square");

  loadTexture(white_pawn,  "white_pawn");
  loadTexture(white_knight,"white_knight");
  loadTexture(white_bishop,"white_bishop");
  loadTexture(white_rook,  "white_rook");
  loadTexture(white_queen, "white_queen");
  loadTexture(white_king,  "white_king");

  loadTexture(black_pawn,  "black_pawn");
  loadTexture(black_knight,"black_knight");
  loadTexture(black_bishop,"black_bishop");
  loadTexture(black_rook,  "black_rook");
  loadTexture(black_queen, "black_queen");
  loadTexture(black_king,  "black_king");

  loadTexture(selection_red,    "selection_red");
  loadTexture(selection_blue,   "selection_blue");
  loadTexture(selection_circle, "selection_circle");

  pthread_create(&render_tid,NULL,renderInThread,NULL);
  pthread_create(&event_tid,NULL,handleEvents,NULL);

  return true;
}

void GraphicsManager::cleanup()
{
  // tell the child to terminate
  childShouldTerminate = true;
  pthread_join(render_tid,NULL);
  pthread_join(event_tid,NULL);
  // cleanup tex array
  for(int i=0; i<TEXTURE_COUNT; i++)
  {
    if(tex[i] != nullptr)
    {
      SDL_DestroyTexture(tex[i]);
    }
  }
  delete [] tex;

  // quit SDL properly
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void GraphicsManager::renderBoard(const Board* board)
{
  pthread_mutex_lock(&render_mutex);
  renderBoardCS(board);
  pthread_mutex_unlock(&render_mutex);
}

void GraphicsManager::renderBoardCS(const Board* board)
{
  lastBoard = board;
  
  SDL_RenderClear(renderer);
  renderTexture(background,NULL);

  SDL_Rect pos;
  pos.w = square_size;
  pos.h = square_size;

  // get and render the input buffer
  pthread_mutex_lock(&event_mutex);
  std::string text = inputBuffer;
  pthread_mutex_unlock(&event_mutex);

  if(gameOver)
  {
    text = gameOverText;
  }

  renderString(text,border_size,
               (border_size * 3) + (square_size * 8) + 
               (textbox_size / 2) - (text_height / 2) );

  std::string playerToMove = "";
  if(board->getPlayerToMove() == White)
  {
    playerToMove = "White to move";
  }
  else
  {
    playerToMove = "Black to move";
  }
  
  std::string statusText = "";

  //statusText += std::to_string( (1 + board->getTurn()) / 2 );
  //statusText += " ";
  //statusText += std::to_string(board->getScore());
  //statusText += " ";
  statusText += playerToMove;

  renderString(statusText,
               (border_size * 2) + (square_size * 8) - 
               (statusText.size() * text_spacing),
               (border_size * 3) + (square_size * 8) + 
               (textbox_size / 2) - (text_height / 2) );

  // render the squares
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      // shifted right a bit to make space for row numbers
      pos.x = xGridToCoord(x);
      pos.y = yGridToCoord(y);

      renderSquare(board,x,y,&pos);
    }
  }

  // render the selection
  // (the squares that highlight when a command is being typed)
  renderSelection(board,text);

  // render the actual pieces
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      // shifted right a bit to make space for row numbers
      pos.x = xGridToCoord(x);
      pos.y = yGridToCoord(y);

      renderPiece(board,x,y,&pos);
    }
  }

  // render the coord system
  for(int y=0; y<8; y++)
  {
    renderCharacter((char)((8 - rotate(y)) + '0'),
                    (2*border_size) - text_spacing - 2,
                    border_size + (square_size * (y+1) ) 
                    - (square_size/2) - (text_height/2));   
  }

  for(int x=0; x<8; x++)
  {
    renderCharacter((char)(rotate(x) + 'a'),
                    (border_size*2) + (square_size * (x+1) ) 
                    - (square_size/2) - (text_width/2),
                    border_size + (square_size * 8));
  }

  SDL_RenderPresent(renderer);
}

int GraphicsManager::rotate(int a)
{
  if(lastBoard->getPlayerToMove() == Black && rotateOnBlack)
  {
    return 7-a;
  }
  else
  {
    return a;
  }
}

int GraphicsManager::xGridToCoord(int x)
{
  return (border_size*2) + ( rotate(x) * square_size);
}

int GraphicsManager::yGridToCoord(int y)
{
  return border_size + ((7-rotate(y)) * square_size);
}

bool GraphicsManager::setRotateOnBlack(bool r)
{
  rotateOnBlack = r;
}

void GraphicsManager::renderSquare
                     (const Board* board,int x,int y,const SDL_Rect* pos)
{
  if(board->getColor(x,y) == WhiteSquare)
  {
    renderTexture(white_square,pos);
  }
  else
  {
    renderTexture(black_square,pos);
  }
}

void GraphicsManager::renderSelection(const Board* board,std::string text)
{
  if(text.compare("o-o") == 0
  || text.compare("O-O") == 0
  || text.compare("0-0") == 0)
  {
    int y = (board->getPlayerToMove() == White ? 0 : 7);
    int begin[2] = {4,y};
    int end[2]   = {6,y};
    if(board->parseNotation(
              (board->getPlayerToMove() == White ? "e1 g1" : "e8 g8"),
              begin,end,nullptr))
    {
      SDL_Rect pos;
      pos.w = square_size;
      pos.h = square_size;

      pos.x = xGridToCoord(5);
      pos.y = yGridToCoord(y);

      renderTexture(selection_circle,&pos);
      pos.x = xGridToCoord(6);
      renderTexture(selection_circle,&pos);
    }
  }
  else if(text.compare("o-o-o") == 0
       || text.compare("O-O-O") == 0
       || text.compare("0-0-0") == 0)
  {
    int y = (board->getPlayerToMove() == White ? 0 : 7);
    int begin[2] = {4,y};
    int end[2]   = {6,y};
    if(board->parseNotation(
              (board->getPlayerToMove() == White ? "e1 c1" : "e8 c8"),
              begin,end,nullptr))
    {
      SDL_Rect pos;
      pos.w = square_size;
      pos.h = square_size;

      pos.x = xGridToCoord(3);
      pos.y = yGridToCoord((board->getPlayerToMove() == White ? 0 : 7));

      renderTexture(selection_circle,&pos);
      pos.x = xGridToCoord(2);
      renderTexture(selection_circle,&pos);
      pos.x = xGridToCoord(1);
      renderTexture(selection_circle,&pos);
    }
  }
  else if(text.size() >= 2)
  {
    SDL_Rect pos;
    pos.w = square_size;
    pos.h = square_size;

    int text_x = text.at(0) - 'a';
    int text_y = text.at(1) - '1';

    if(board->areValidCoordinates(text_x,text_y))
    {
      pos.x = xGridToCoord(text_x);
      pos.y = yGridToCoord(text_y);

      renderTexture(selection_red,&pos);
    }

    std::vector<Coord> possibleMoves = 
    BoardUtils::getPieceMoves(board,text_x,text_y);

    if(!possibleMoves.empty())
    {
      for(auto it = possibleMoves.begin(); it != possibleMoves.end(); ++it)
      {
        //std::cout << (char)(it->x + 'a') << (char)(it->y + '1') << std::endl;
        pos.x = xGridToCoord(it->x);
        pos.y = yGridToCoord(it->y);

        renderTexture(selection_circle,&pos);
      }
    }

    if(text.size() >= 5)
    {
      int text_x_end = text.at(3) - 'a';
      int text_y_end = text.at(4) - '1';
      if(board->areValidCoordinates(text_x_end,text_y_end)
      && (text_x != text_x_end || text_y != text_y_end) )
      {
        pos.x = xGridToCoord(text_x_end);
        pos.y = yGridToCoord(text_y_end);

        renderTexture(selection_blue,&pos);
      }
    }
  }
}

void GraphicsManager::renderPiece
                      (const Board* board,int x,int y,const SDL_Rect* pos)
{ 
  if(board->getPiece(x,y) != Empty)
  {
    int tex_id = white_pawn;
    switch(board->getPiece(x,y))
    {
      case Pawn:
        tex_id = white_pawn;
        break;
      case Knight:
        tex_id = white_knight;
        break;
      case Bishop:
        tex_id = white_bishop;
        break;
      case Rook:
        tex_id = white_rook;
        break;
      case Queen:
        tex_id = white_queen;
        break;
      case King:
        tex_id = white_king;
        break;
    }

    if(board->getPlayer(x,y) == Player::Black)
    {
      tex_id += (black_pawn - white_pawn);
    }

    renderTexture((Texture)tex_id,pos);
  }
}

void GraphicsManager::renderTexture(Texture t,const SDL_Rect* pos)
{
  SDL_RenderCopy(renderer,tex[t],NULL,pos);
}

void GraphicsManager::renderString(std::string str,int x,int y)
{
  for(int i=0; i<str.size(); i++)
  {
    renderCharacter(str.at(i),x + (i * text_spacing),y);
  }
}

void GraphicsManager::renderCharacter(char c,int x,int y)
{
  SDL_Rect src;
  src.x = ((int)c % 32) * text_width;
  src.y = ((int)c / 32) * text_height;
  src.w = text_width;
  src.h = text_height;

  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = text_width;
  dest.h = text_height;

  SDL_RenderCopy(renderer,tex[text],&src,&dest);
}

bool GraphicsManager::loadTexture(Texture t,std::string filename)
{
  std::string filepath = "res/" + filename + ".bmp";
  SDL_Surface* temp = SDL_LoadBMP(filepath.c_str());
  if(temp == nullptr)
  {
    std::cerr << "Failed to create SDL surface: ";
    std::cerr << SDL_GetError() << std::endl;
    cleanup();
    return false;
  }

  tex[t] = SDL_CreateTextureFromSurface(renderer,temp);
  SDL_FreeSurface(temp);
  if(tex[t] == nullptr)
  {
    std::cerr << "Failed to create SDL texture: ";
    std::cerr << SDL_GetError() << std::endl;
    cleanup();
    return false;
  }
  
  return true;
}

void* GraphicsManager::renderInThread(void* args)
{
  bool running = true;
  while(running)
  {
    pthread_mutex_lock(&render_mutex);
    if(lastBoard != nullptr)
    {
      renderBoardCS(lastBoard);
      running = !childShouldTerminate;
    }
    pthread_mutex_unlock(&render_mutex);

    SDL_Delay(100);
  }
}

void* GraphicsManager::handleEvents(void* args)
{
  bool running = true;
  SDL_Event e;
  const char* fullkey;
  char key;

  while(running)
  {
    pthread_mutex_lock(&event_mutex);
    if(SDL_PollEvent(&e))
    {
      if(e.type == SDL_QUIT)
      {
        shouldQuit = true; 
      }
      else if(e.type == SDL_KEYDOWN)
      {
        //std::cout << SDL_GetKeyName(e.key.keysym.sym)[0] << std::endl;
        if(e.key.keysym.sym == SDLK_RETURN)
        {
          //std::cout << "RETURN" << std::endl;
          timeToReadInput = true;
        }
        else if(e.key.keysym.sym == SDLK_BACKSPACE)
        {
          inputBuffer = inputBuffer.substr(0,inputBuffer.size()-1);
        }
        else if(e.key.keysym.sym == SDLK_SPACE)
        {
          inputBuffer.append(1,' ');
        }
        else
        {
          fullkey = SDL_GetKeyName(e.key.keysym.sym);
          //std::cout << "fullkey = " << fullkey << std::endl;
          key = fullkey[0];
          if( (key == 'Q' || key == 'C' ) && e.key.keysym.mod == KMOD_LCTRL)
          {
            // quit the program
            inputBuffer = "Goodbye!";
            shouldQuit = true;
          }
          else if(strlen(fullkey) == 1 && key >= 'A' && key <= 'Z')
          {
            inputBuffer.append(1,key - 'A' + 'a');
          }
          else if(strlen(fullkey) == 1 && key >= '!' && key <= '~')
          {
            inputBuffer.append(1,key);
          }
        }
      }
      else if(e.type == SDL_MOUSEBUTTONDOWN && 
              e.button.button == SDL_BUTTON_LEFT)
      {
        int mouse_x =      ( e.button.x - (border_size * 2)) / square_size;
        int mouse_y = 7 - (( e.button.y -  border_size     ) / square_size);

        mouse_x = rotate(mouse_x);
        mouse_y = rotate(mouse_y);

        switch(inputBuffer.size())
        {
          case 0:
            inputBuffer.append(1,(char)(mouse_x + 'a'));
            inputBuffer.append(1,(char)(mouse_y + '1'));
            inputBuffer.append(1,'-');
            break;
          case 3:
            inputBuffer.append(1,(char)(mouse_x + 'a'));
            inputBuffer.append(1,(char)(mouse_y + '1'));
            timeToReadInput = true;
            break;
        }
      }
    }
    running = !childShouldTerminate;
    pthread_mutex_unlock(&event_mutex);

    SDL_Delay(10);
  }
}

std::string GraphicsManager::getInput()
{
  std::string retval;
  bool ready = false;

  while(!ready && !shouldQuit)
  {
    pthread_mutex_lock(&event_mutex);

    ready = timeToReadInput;
    retval = inputBuffer;
    if(ready)
    {
      timeToReadInput = false;
      inputBuffer = "";
    }

    pthread_mutex_unlock(&event_mutex);

    SDL_Delay(30);
  }

  //std::cout << "getInput() return " << retval << std::endl;
  return retval; 
}

bool GraphicsManager::programTerminated()
{
  pthread_mutex_lock(&event_mutex);
  bool retval = shouldQuit;
  pthread_mutex_unlock(&event_mutex);
  return retval;
}

void GraphicsManager::setGameOver(std::string str)
{
  gameOverText = str;
  gameOver = true;
}
