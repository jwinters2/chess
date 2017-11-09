#include "TrainingData.h"

#include "Board.h"
#include "Enums.h"
#include "PieceLogic.h"

#include <fstream>
#include <cstdio>
#include <cstdlib>

#include <cassert>
#define NDEBUG
//#define DEBUG_PRINT

TrainingData::TrainingData(int b,int e,char** args)
                           :beginningIndex(b),endingIndex(e),arguments(args)
{
}

void TrainingData::parse() const
{
  std::ifstream currentFile;
  std::ofstream outFile;
  std::string whiteMove, blackMove, currentWord;
  int gameCount = 1;
  char buffer[256];

  system( "mkdir training_data" );
  //system(("mkdir training_data/set_" + intToString(1)).c_str());
  //outFile.open("training_data/set_" + intToString(1) + "/move_" + 
  //             intToString(gameCount) + ".data");
  outFile.open("training_data/set_" + intToString(1) + ".data");

  for(int i=beginningIndex; i<endingIndex; i++)
  {
    currentFile.open(arguments[i]);
    if(currentFile.is_open())
    {
      while(!currentFile.eof())
      {
        while(!currentFile.eof() && currentFile.peek() != '1')
        {
          #ifdef DEBUG_PRINT
          if(buffer[0] != '\0')
          {
            std::cout << "skipping metadata: " << buffer << std::endl;
          }
          #endif
          currentFile.getline(buffer,256);
        }

        if(currentFile.eof())
        {
          continue;
        }


        //whiteMove = currentWord;
        currentFile >> whiteMove;
        currentFile >> blackMove;

        Board board, tempBoard;
        Move move;
        bool success = true;

        do
        {
          if(isEndingMove(whiteMove))
          {
            break;
          }
          removeNumber(whiteMove);
          if(whiteMove.empty())
          {
            currentFile >> whiteMove;
          }
          //outFile << board << std::endl;
          //outFile << convertNotation(&board,whiteMove) << std::endl;
          move = convertNotation(&tempBoard,whiteMove,success);
          if(!success)
          {
            break;
          }
          printBoard(outFile,board);
          printMove(outFile,move);
          board.makeIntoCopyOf(tempBoard);
          saveToFile(outFile,success,gameCount);

          if(isEndingMove(blackMove))
          {
            break;
          }
          removeNumber(blackMove);
          //outFile << board << std::endl;
          //outFile << convertNotation(&board,blackMove) << std::endl;
          move = convertNotation(&tempBoard,blackMove,success);
          if(!success)
          {
            break;
          }
          printBoard(outFile,board);
          printMove(outFile,move);
          board.makeIntoCopyOf(tempBoard);
          saveToFile(outFile,success,gameCount);

          if(whiteMove[0] == '[' || blackMove[0] == '[')
          {
            break;
          }
        }
        while(currentFile >> whiteMove >> blackMove);
         
      }
      currentFile.close();
    }
  }
}

bool TrainingData::beginsWith(std::string a,std::string b)
{
  if(a.size() < b.size())
  {
    return false;
  }

  for(int i=0; i<b.size(); i++)
  {
    if(a[i] != b[i])
    {
      return false;
    }
  }

  return true;
}

void TrainingData::removeNumber(std::string& str)
{
  if(str.empty())
  {
    return;
  }

  while((str[0] >= '0' && str[0] <= '9') || str[0] == '.')
  {
    str = str.substr(1,str.size() - 1);
  }
}

bool TrainingData::isEndingMove(std::string str)
{
  //return (str.compare("1-0") == 0
       //|| str.compare("0-1") == 0 
       //|| str.compare("1/2-1/2") == 0
       //|| str.compare("*") == 0);

  return (beginsWith(str,"1-0")
       || beginsWith(str,"0-1")
       || beginsWith(str,"1/2-1/2")
       || beginsWith(str,"*"));
}

bool TrainingData::isInRange(char c,char a,char b)
{
  return (c >= a && c <= b);
}

std::string TrainingData::intToString(int a)
{
  std::string zeros = "00000";
  int temp = a;
  while (temp != 0)
  {
    zeros = zeros.substr(1,zeros.size()-1);
    temp /= 10;
  }
  return zeros + std::to_string(a);
}

std::string TrainingData::clearPath(std::string str)
{
  // portability
  #ifdef __unix__
  char c = '/';
  #elif _WIN32
  char c = '\\';
  #endif
  while(contains(str,c))
  {
    str = str.substr(1,str.size()-1);
  }
  return str;
}

bool TrainingData::contains(std::string str,char c)
{
  for(int i=0; i<str.size(); i++)
  {
    if(str[i] == c)
    {
      return true;
    }
  }

  return false;
}

Move TrainingData::convertNotation(Board* board,std::string str,bool& success)
{
  success = true;
  Piece piece = Pawn;
  bool endSquareFound = false;

  Move retval(-1,-1,-1,-1);

  for(int i=0; i<str.size()-1; i++)
  {
    if(isInRange(str[i],'A','Z'))
    {
      // a capital letter is the piece that's moving
      // no piece specified means a pawn, so keep the default value
      switch(str[i])
      {
        case 'N':
          piece = Knight;
          break;
        case 'B':
          piece = Bishop;
          break;
        case 'R':
          piece = Rook;
          break;
        case 'Q':
          piece = Queen;
          break;
        case 'K':
          piece = King;
          break;
      }
    }
    else if(isInRange(str[i],'a','h') && isInRange(str[i+1],'1','8'))
    {
      // a simple letter-number combination

      if(endSquareFound)
      {
        // we already "found" the end, meaning the last pair was actually
        // the rank and file of the previous one, i.e. the beginning

        retval.start_x = retval.end_x;
        retval.start_y = retval.end_y;
      }

      retval.end_x = str[i]   - 'a';
      retval.end_y = str[i+1] - '1';

      endSquareFound = true;

      // we skip 2 places instead of 1
      i++;
    }
    else if(isInRange(str[i],'a','h'))
    {
      // a lone letter is the file (x coordinate)
      retval.start_x = str[i] - 'a';
    }
    else if(isInRange(str[i],'1','8'))
    {
      // a lone number is the rank (y coordinate)
      retval.start_y = str[i] - '1';
    }
    else if(str[i] == '=' && isInRange(str[i+1],'A','Z'))
    {
      // an equals sign and a capital letter means the equals sign
      switch(str[i+1])
      {
        case 'N':
          retval.promote = Knight;
          break;
        case 'B':
          retval.promote = Bishop;
          break;
        case 'R':
          retval.promote = Rook;
          break;
        case 'Q':
          retval.promote = Queen;
          break;
      }

      // we skip 2 places instead of 1
      i++;
    }
  }
  
  // parse castling notation
  if(beginsWith(str,"O-O"))
  {
    // castling never promotes
    retval.promote = Empty;

    // kings always start at column x=4 and can't move
    retval.start_x = 4;

    if(board->getPlayerToMove() == White)
    {
      // white castling
      retval.start_y = 0;
      retval.end_y   = 0;
    }
    else
    {
      // black castling
      retval.start_y = 7;
      retval.end_y   = 7;
    }

    if(beginsWith(str,"O-O-O"))
    {
      retval.end_x = 2;
    }
    else
    {
      retval.end_x = 6;
    }
  }

  int begin_arr[2];
  int end_arr[2] = { retval.end_x , retval.end_y };

  // determine the rest of the data from the board
  if(retval.start_x == -1 && retval.start_y == -1)
  {
    // no file or rank specified, go through the entire board 
    for(int x=0; x<8; x++)
    {
      for(int y=0; y<8; y++)
      {
        if(board->getPiece(x,y) == piece
        && board->getPlayer(x,y) == board->getPlayerToMove())
        {
          begin_arr[0] = x;
          begin_arr[1] = y;
          if(PieceLogic::isMoveValid(board,begin_arr,end_arr,
                                     retval.promote,nullptr))
          {
            retval.start_x = x;
            retval.start_y = y;

            #ifdef DEBUG_PRINT
            std::cout << "xy " << str << " (" << (char)piece << ") = ";
            std::cout << retval << std::endl;
            #endif
            assert(board->move(retval));
            return retval;
          }
        }
      }
    }
  }
  else if(retval.start_y == -1)
  {
    // file specified but not rank, go through only this rank

    begin_arr[0] = retval.start_x;

    for(int y=0; y<8; y++)
    {
      if(board->getPiece(retval.start_x , y) == piece
      && board->getPlayer(retval.start_x , y) == board->getPlayerToMove())
      {
        begin_arr[1] = y;
        if(PieceLogic::isMoveValid(board,begin_arr,end_arr,
                                   retval.promote,nullptr))
        {
          retval.start_y = y;
          #ifdef DEBUG_PRINT
          std::cout << "y " << str << " (" << (char)piece << ") = ";
          std::cout << retval << std::endl;
          #endif
          assert(board->move(retval));
          return retval;
        }
      }
    }
  }
  else if(retval.start_x == -1)
  {
    // rank specified but not file, go through only this file

    begin_arr[1] = retval.start_y;

    for(int x=0; x<8; x++)
    {
      if(board->getPiece(x,retval.start_y) == piece
      && board->getPlayer(x,retval.start_y) == board->getPlayerToMove())
      {
        begin_arr[0] = x;
        if(PieceLogic::isMoveValid(board,begin_arr,end_arr,
                                   retval.promote,nullptr))
        {
          retval.start_x = x;
          #ifdef DEBUG_PRINT
          std::cout << "x " << str << " (" << (char)piece << ") = ";
          std::cout << retval << std::endl;
          #endif
          assert(board->move(retval));
          return retval;
        }
      }
    }
  }

  #ifdef DEBUG_PRINT
  std::cout << str << " (" << (char)piece << ") = ";
  std::cout << retval << std::endl;
  #endif
  begin_arr[0] = retval.start_x;
  begin_arr[1] = retval.start_y;
  if(retval.start_x == -1 || retval.start_y == -1
  || !PieceLogic::isMoveValid(board,begin_arr,end_arr,
                             retval.promote,nullptr))
  {
    success = false;
    return retval;
  }
  assert(board->move(retval));
  return retval;
}

void TrainingData::printBoard(std::ostream& out,const Board& board)
{
  out << (board.getPlayerToMove() == White ? "1" : "0") << std::endl;
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      if(board.getPiece(x,y) == Empty)
      {
        out << "__";
      }
      else
      {
        if(board.getPlayer(x,y) == White)
        {
          out << "w";
        }
        else
        {
          out << "b";
        }

        switch(board.getPiece(x,y))
        {
          case Pawn:
            out << "p";
            break;
          case Knight:
            out << "n";
            break;
          case Bishop:
            out << "b";
            break;
          case Rook:
            out << "r";
            break;
          case Queen:
            out << "q";
            break;
          case King:
            out << "k";
            break;
        }
      }

      out << " ";
    }
    out << std::endl;
  }
  out << std::endl;
}

void TrainingData::printMove(std::ostream& out,const Move& m)
{
  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      out << (m.start_x == x && m.start_y == y ? "1 " : "0 ");
    }
    out << std::endl;
  }
  out << std::endl;

  for(int x=0; x<8; x++)
  {
    for(int y=0; y<8; y++)
    {
      out << (m.end_x == x && m.end_y == y ? "1 " : "0 ");
    }
    out << std::endl;
  }
  out << std::endl;
}

void TrainingData::saveToFile(std::ofstream& out,bool success,int& count)
{
  if(out.is_open())
  {
    //int moveNum = (count % 10000) + 1;
    int folderNum = (count / 10000) + 1;
    std::string outFileName = "training_data/set_" + 
                              intToString(folderNum) + ".data";
                              //intToString(moveNum) + ".data";

    // if the game was parsed successfully, increment the count
    if(success)
    {
      count++;
    }
    else
    {
      // otherwise, delete the file
      remove(outFileName.c_str());
    }

    // make a new folder if we have to
    if(folderNum != (count / 10000) + 1)
    {
      out.close();

      folderNum = (count / 10000) + 1;
      //system(("mkdir training_data/set_" + intToString(folderNum)).c_str());
      std::cout << "now writing to training_data/set_";
      std::cout << intToString(folderNum) << std::endl;

      outFileName = "training_data/set_" + intToString(folderNum) + ".data";
                  //intToString(moveNum) + ".data";

      out.open(outFileName);
    }
  }
}
