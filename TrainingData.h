#pragma once

#include "Board.h"
#include "Enums.h"

#include <string>

class TrainingData
{
  public:
    TrainingData(int,int,char**);
    void parse() const;

    static std::string intToString(int);

  private:
    int beginningIndex;
    int endingIndex;
    char** arguments;

    static bool beginsWith(std::string,std::string);
    static void removeNumber(std::string&);
    static bool isEndingMove(std::string);
    static bool isInRange(char,char,char);
    static std::string clearPath(std::string);
    static bool contains(std::string,char);
    static Move convertNotation(Board*,std::string,bool&);
    static void printBoard(std::ostream&,const Board&);
    static void printMove(std::ostream&,const Move&);
    static void saveToFile(std::ofstream&,bool,int&);
};
