CC=g++
CFLAGS=--std=c++11 -O3 -Wall
LDFLAGS=-lSDL2 -lpthread
SOURCES=Enums.cpp Square.cpp Board.cpp BoardUtils.cpp PieceLogic.cpp GraphicsManager.cpp Opponent.cpp TrainingData.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=chess

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm *.o chess
