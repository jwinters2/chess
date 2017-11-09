#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <pthread.h>

#include "Enums.h"

enum Texture
{
  background = 0,
  text,
  white_square,
  black_square,
  white_pawn,
  white_knight,
  white_bishop,
  white_rook,
  white_queen,
  white_king,
  black_pawn,
  black_knight,
  black_bishop,
  black_rook,
  black_queen,
  black_king,
  selection_red,
  selection_blue,
  selection_circle,
  promotion_menu,
  TEXTURE_COUNT
};

class Board;
class Square;

class GraphicsManager
{
  public:
    static bool init();
    static void cleanup();
    static void renderBoard(const Board*);

    static std::string getInput();
    static Piece getPromotion();
    static bool programTerminated();

    static void setRotateOnBlack(bool);

    static void setGameOver(std::string);

  private:
    //GraphicsManager();
    //~GraphicsManager();

    static const Board* lastBoard;
    static pthread_mutex_t render_mutex;
    static pthread_mutex_t event_mutex;
    static pthread_t render_tid;
    static pthread_t event_tid;
    static bool childShouldTerminate;

    static std::string inputBuffer;
    static bool timeToReadInput;
    static bool displayPromotionMenu;
    static Piece promotionPiece;
    static bool shouldQuit;

    static bool gameOver;
    static std::string gameOverText;

    static const int square_size;
    static const int border_size;
    static const int textbox_size;

    static const int text_width;
    static const int text_height;
    static const int text_spacing;

    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static SDL_Texture** tex;

    static void renderSquare(const Board*,int,int,const SDL_Rect*);
    static void renderSelection(const Board*,std::string);
    static void renderPiece(const Board*,int,int,const SDL_Rect*);
    static void renderTexture(Texture,const SDL_Rect*);

    static void renderString(std::string,int,int);
    static void renderCharacter(char,int,int);

    static int rotate(int);
    static int xGridToCoord(int);
    static int yGridToCoord(int);
    static bool rotateOnBlack;

    static void renderBoardCS(const Board*);
    static void* renderInThread(void*);
    static void* handleEvents(void*);

    static bool loadTexture(Texture,std::string);
};
