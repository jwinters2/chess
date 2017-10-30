This is a simple chess game.

INSTRUCTIONS:

To compile:

    $ g++ --std=c++11 *.cpp -lSDL2 -lpthread -O3 -o chess

  make sure you have SDL 2.0 installed, on linux the command is:

    (Debian / Ubuntu) $ sudo apt-get install libsdl2-dev

    (Arch)            $ sudo pacman -S sdl2

    (Other)           $ google it please


To run:

    ./chess [ -r | -d DEPTH | -c | -h | -o ]

  -r will rotate the board when it's black's turn, so
  both players will be playing from the bottom.

  -d will set the search depth of the computer opponent,
  the higher d is, the smarter and slower it is.  The default value is 3.

  -h will set the opponent to be a second human and not the computer

  -o will setup the board according to a file rather than the starting position.
  these files are stored in boardstates/, end in .bs and use the following
  syntax:

    <x> <y> <color> <piece>

  so "a 2 w q" means a white queen at square a2.

To play:

  moves are in the format:
    
    "a1-b2" or "a1 b2"

  which will move the piece in square a1 to square b2

  Castling is either "O-O" for kingside, and "O-O-O" for queenside.
  Lowercase O's ("o") and zeros ("0") will also work.

To exit:
  
  Ctrl-c or Ctrl-q
  
To do (for me):

- a better chess A.I. to play against

- fix memory leaks (SDL 2.0 is weird, literally opening and closing it loses
  like 300 bytes, and somehow leaving it open loses less)

- maybe have a neural network learn off the basic A.I.  I doubt this will work
  but why not give it a go?
