#pragma once

#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>

#include "Enums.h"
#include "Board.h"

class NeuralNetwork
{
  public:
    NeuralNetwork(int,int,int,int,int,std::string);
    ~NeuralNetwork();

    Move getMove(const Board*);
    void train(float);

  private:
    static const int miniBatchSize;
    static const int exampleByteCount;
    static const int maxExamplesPerFile;
    static const int fileCount;

    const int inputLayerSize;
    const int outputLayerSize;
    const int hiddenLayerSize;
    const int hiddenLayerCount;
    const int folderCount;
    const std::string filename;

    std::fstream brainFile;
    void loadFromFile();
    void saveToFile();
    void readWriteToFile(bool);
    void readWriteData(std::fstream&,float&,bool);
    void readWriteData(std::fstream&,int&,bool);

    time_t timeToEnd;

    float* inputLayerActivation;
    float* outputLayerActivation;
    float** hiddenLayerActivation;

    float* desiredOutput;

    float* outputLayerBias;
    float** hiddenLayerBias;
    float* outputLayerBiasChanges;
    float** hiddenLayerBiasChanges;

    float** inputToHiddenWeights;
    float** hiddenToOutputWeights;
    float*** hiddenToHiddenWeights;
    float** inputToHiddenWeightChanges;
    float** hiddenToOutputWeightChanges;
    float*** hiddenToHiddenWeightChanges;

    float* inputLayerDelta;
    float* outputLayerDelta;
    float** hiddenLayerDelta;

    static float sigmoid(float);
    static float sigprime(float);

    void setInputLayer(const Board*);
    void loadRandomExample();

    void forwardPropogate();  
    void backwardPropogate(float);  

    void matrixMultiply(float*,float**,float*);

    void resetChanges();
};
