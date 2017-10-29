This is a simple chess game.

INSTRUCTIONS:

To compile:

    $ g++ *.cpp -lSDL2 -lpthread -o chess

  make sure you have SDL 2.0 installed, on linux the command is:

    (Debian / Ubuntu) $ sudo apt-get install libsdl2-dev

    (Arch)            $ sudo pacman -S sdl2

    (Other)           $ google it please


To run:

    ./chess [ -r | -d DEPTH | -c | -h ]

  the -r option will rotate the board when it's black's turn, so
  both players will be playing from the bottom.

  the -d option will set the search depth of the computer opponent,
  the higher d is, the smarter and slower it is.  The default value is 3

  -c and -h will set the opponent to be a computer or a human respectively

To play:

  moves are in the format:
    
    "a1-b2" or "a1 b2"

  which will move the piece in square a1 to square b2

  Castling is either "O-O" for kingside, and "O-O-O" for queenside.
  Lowercase O's ("o") and zeros ("0") will also work.

To exit:
  
  Ctrl-c or Ctrl-q
  
To do (for me):

- support stricter rules for castling
  (the king can't pass through check when castling in 
  official chess, but can here)

- prevent the king from moving into check

- tell when the game is over

- detect stalemate and consider it a draw

- a better chess A.I. to play against

- fix memory leaks (SDL 2.0 is weird, literally opening and closing it loses
  like 300 bytes, and somehow leaving it open loses less)

- maybe have a neural network learn off the basic A.I.  I doubt this will work
  but why not give it a go?
