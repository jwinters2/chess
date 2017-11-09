#include "NeuralNetwork.h"
#include "TrainingData.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

const int NeuralNetwork::miniBatchSize      = 200;
const int NeuralNetwork::exampleByteCount   = 477;
const int NeuralNetwork::maxExamplesPerFile = 10000;
const int NeuralNetwork::fileCount          = 16;

NeuralNetwork::NeuralNetwork(int hs,int hc,int fc,std::string f):
                             inputLayerSize(1 + (64*7)),outputLayerSize(64*2),
                             hiddenLayerSize(hs),hiddenLayerCount((hc>2?hc:2)),
                             folderCount(fc),filename(f)
{
  // allocate arrays for input layers
  inputLayerActivation       = new float[inputLayerSize];
  inputLayerDelta            = new float[inputLayerSize];
  inputToHiddenWeights       = new float*[inputLayerSize];
  inputToHiddenWeightChanges = new float*[inputLayerSize];
  for(int i=0; i<inputLayerSize; i++)
  {
    inputToHiddenWeights      [i] = new float[hiddenLayerSize];
    inputToHiddenWeightChanges[i] = new float[hiddenLayerSize];
  }

  // allocate arrays for output layers
  outputLayerActivation       = new float[outputLayerSize];
  desiredOutput               = new float[outputLayerSize];
  outputLayerBias             = new float[outputLayerSize];
  outputLayerBiasChanges      = new float[outputLayerSize];
  outputLayerDelta            = new float[outputLayerSize];
  hiddenToOutputWeights       = new float*[hiddenLayerSize];
  hiddenToOutputWeightChanges = new float*[hiddenLayerSize];
  for(int i=0; i<hiddenLayerSize; i++)
  {
    hiddenToOutputWeights      [i] = new float[outputLayerSize];
    hiddenToOutputWeightChanges[i] = new float[outputLayerSize];
  }

  // allocate arrays for hidden layers
  hiddenLayerActivation  = new float*[hiddenLayerCount];
  hiddenLayerBias        = new float*[hiddenLayerCount];
  hiddenLayerBiasChanges = new float*[hiddenLayerCount];
  hiddenLayerDelta       = new float*[hiddenLayerCount];
  for(int i=0; i<hiddenLayerCount; i++)
  {
    hiddenLayerActivation [i] = new float[hiddenLayerSize];
    hiddenLayerBias       [i] = new float[hiddenLayerSize];
    hiddenLayerBiasChanges[i] = new float[hiddenLayerSize];
    hiddenLayerDelta      [i] = new float[hiddenLayerSize];
  }

  hiddenToHiddenWeights       = new float**[hiddenLayerCount-1];
  hiddenToHiddenWeightChanges = new float**[hiddenLayerCount-1];
  for(int i=0; i<hiddenLayerCount-1; i++)
  {
    hiddenToHiddenWeights      [i] = new float*[hiddenLayerSize];
    hiddenToHiddenWeightChanges[i] = new float*[hiddenLayerSize];
    for(int j=0; j<hiddenLayerSize; j++)
    {
      hiddenToHiddenWeights      [i][j] = new float[hiddenLayerSize];
      hiddenToHiddenWeightChanges[i][j] = new float[hiddenLayerSize];
    }
  }

  // seed the RNG
  srand(time(NULL));

  // load the "brain" from the file
  loadFromFile();
}

NeuralNetwork::~NeuralNetwork()
{
  // free arrays for input layers
  for(int i=0; i<inputLayerSize; i++)
  {
    delete [] inputToHiddenWeights[i];
    delete [] inputToHiddenWeightChanges[i];
  }
  delete [] inputLayerActivation;
  delete [] inputLayerDelta;
  delete [] inputToHiddenWeights;
  delete [] inputToHiddenWeightChanges;

  // free arrays for output layers
  for(int i=0; i<hiddenLayerSize; i++)
  {
    delete [] hiddenToOutputWeights[i];
    delete [] hiddenToOutputWeightChanges[i];
  }
  delete [] outputLayerActivation;
  delete [] desiredOutput;
  delete [] outputLayerBias;
  delete [] outputLayerBiasChanges;
  delete [] outputLayerDelta;
  delete [] hiddenToOutputWeights;
  delete [] hiddenToOutputWeightChanges;

  // allocate arrays for hidden layers
  for(int i=0; i<hiddenLayerCount; i++)
  {
    delete [] hiddenLayerActivation[i];
    delete [] hiddenLayerBias[i];
    delete [] hiddenLayerBiasChanges[i];
    delete [] hiddenLayerDelta[i];
  }
  delete [] hiddenLayerActivation;
  delete [] hiddenLayerBias;
  delete [] hiddenLayerBiasChanges;
  delete [] hiddenLayerDelta;

  for(int i=0; i<hiddenLayerCount-1; i++)
  {
    for(int j=0; j<hiddenLayerSize; j++)
    {
      delete [] hiddenToHiddenWeights[i][j];
      delete [] hiddenToHiddenWeightChanges[i][j];
    }
    delete [] hiddenToHiddenWeights[i];
    delete [] hiddenToHiddenWeightChanges[i];
  }
  delete [] hiddenToHiddenWeights;
  delete [] hiddenToHiddenWeightChanges;
}

Move NeuralNetwork::getMove(const Board* board)
{
  setInputLayer(board);
  forwardPropogate();
  // TODO: find most active move and return it
  return Move();
}

void NeuralNetwork::train(float hours)
{
  timeToEnd = time(NULL) + (hours * 3600.0f);
  float cost;
  int exampleCount = 0;

  // repeat until we run out of time
  while(time(NULL) < timeToEnd)
  {
    std::cout << "training on minibatch #" << exampleCount++ << std::endl;
    // keep going through example files in a random folder until we run out
    resetChanges();

    for(int i=0; i<miniBatchSize; i++)
    {
      loadRandomExample();
      forwardPropogate();
      cost = getCostOfExample();

      std::cout << "cost = " << cost << std::endl;

      //backwardPropogate(cost);
      //calculateChanges();
    }

    // now each change are the average of all changes and not the sum
    divideChangesBy(exampleCount);
    applyChanges();
    saveToFile();
  }

  std::cout << "no more time for training, hope I'm good enough" << std::endl;
}

// sets the change arrays / matrices for biases / weights to 0.0f
void NeuralNetwork::resetChanges()
{
  // biases
  // output
  memset(outputLayerBiasChanges,0,sizeof(float) * outputLayerSize);

  // hidden
  for(int i=0; i<hiddenLayerCount; i++)
  {
    memset(HiddenLayerBiasChanges[i],0,sizeof(float) * hiddenLayerSize);
  }

  // weights
  //input -> hidden
  for(int i=0; i<inputLayerSize; i++)
  {
    memset(inputToHiddenWeightChanges[i],0,sizeof(float) * hiddenLayerSize);
  }

  // hidden -> hidden
  for(int i=0; i<hiddenLayerCount-1; i++)
  {
    for(int j=0; j<hiddenLayerSize; j++)
    {
      memset(hiddenToHiddenWeightChanges[i][j],0,
             sizeof(float) * hiddenLayerSize);
    }
  }

  // hidden -> output
  for(int i=0; i<hiddenLayerSize; i++)
  {
    memset(hiddenToOutputWeightChanges[i],0,sizeof(float) * outputLayerSize);
  }
}

void NeuralNetwork::setInputLayerFromFile()
{
}

void NeuralNetwork::loadFromFile()
{
  readWriteToFile(false);
}

void NeuralNetwork::saveToFile()
{
  readWriteToFile(true);
}

void NeuralNetwork::readWriteToFile(bool write)
{
  // this is one function to guarantee that reading from and writing to
  // the file is synchronized (i.e., variables are in a consistant order)

  if(brainFile.is_open())
  {
    brainFile.seekp(0);
  }
  else
  {
    brainFile.open("brain_file");
  }

  if(!brainFile.is_open())
  {
    std::cerr << "Error: brain_file could not be opened" << std::endl;
    return;
  }

  // hidden layer biases
  for(int i=0; i<hiddenLayerCount; i++)
  {
    for(int j=0; j<hiddenLayerSize; j++)
    {
      readWriteData(brainFile,hiddenLayerBias[i][j],write);
    }
  }

  // output layer biases
  for(int i=0; i<outputLayerSize; i++)
  {
    readWriteData(brainFile,outputLayerBias[i],write);
  }

  // input -> hidden weights
  for(int i=0; i<inputLayerSize; i++)
  {
    for(int j=0; j<hiddenLayerSize; j++)
    {
      readWriteData(brainFile,inputToHiddenWeights[i][j],write);
    }
  }

  // hidden -> hidden weights
  for(int i=0; i<hiddenLayerSize; i++)
  {
    for(int j=0; j<hiddenLayerSize; j++)
    {
      for(int k=0; k<hiddenLayerSize; k++)
      {
        readWriteData(brainFile,hiddenToHiddenWeights[i][j][k],write);
      }
    }
  }

  // hidden -> output weights
  for(int i=0; i<hiddenLayerSize; i++)
  {
    for(int j=0; j<outputLayerSize; j++)
    {
      readWriteData(brainFile,hiddenToOutputWeights[i][j],write);
    }
  }
}

void NeuralNetwork::readWriteData(std::fstream& file,float& f,bool write)
{
  if(write)
  {
    file << f;
  }
  else
  {
    file >> f;    
  }
}

void NeuralNetwork::readWriteData(std::fstream& file,int& i,bool write)
{
  if(write)
  {
    file << i;
  }
  else
  {
    file >> i;    
  }
}

void loadRandomExample()
{
  int fileNum;
  int pos;
  std::ifstream file;
  std::string buffer;

  do
  {
    if(file.is_open())
    {
      file.close();
    }

    fileNum = (rand() % fileCount) + 1;
    file.open(("training_data/set_" + TrainingData::intToString(fileNum)
               + ".data").c_str());
    pos = (rand() % maxExamplesPerFile) * exampleByteCount;
    file.seekg(pos);
  }
  while(file.eof());

  // one neuron for whose turn it is, 1 = white, 0 = black
  file >> inputLayerActivation[0];

  // each square has 7 neurons
  // (0) = Player who owns the square (1 = white, 0 = black, 0.5 = empty square)
  // (1) = Is the piece a pawn (1 = yes, 0 = no)
  // (2) = Is the piece a knight
  // (3) = Is the piece a bishop
  // (4) = Is the piece a rook
  // (5) = Is the piece a queen
  // (6) = Is the piece a king
  for(int i=0; i<64; i++)
  {
    file >> buffer;
    if(strcmp(buffer,"__") == 0)
    {
      // empty square = no owner, no piece
      inputLayerActivation[(7*i) + 1] = 0.5f;
      for(int j=2; j<=7; j++)
      {
        inputLayerActivation[(7*i) + j] = 0.0f;
      }
    }
    else
    {
      // square is occupied
      inputLayerActivation[(7*i) + 1] = (buffer[0] == 'w' ? 1.0f : 0.0f);

      inputLayerActivation[(7*i) + 2] = (buffer[1] == 'p' ? 1.0f : 0.0f);
      inputLayerActivation[(7*i) + 3] = (buffer[1] == 'n' ? 1.0f : 0.0f);
      inputLayerActivation[(7*i) + 4] = (buffer[1] == 'b' ? 1.0f : 0.0f);
      inputLayerActivation[(7*i) + 5] = (buffer[1] == 'r' ? 1.0f : 0.0f);
      inputLayerActivation[(7*i) + 6] = (buffer[1] == 'q' ? 1.0f : 0.0f);
      inputLayerActivation[(7*i) + 7] = (buffer[1] == 'k' ? 1.0f : 0.0f);
    }
  }

  for(int i=0; i<128; i++)
  {
    file >> buffer;
    desiredOutput[i] = (buffer[0] == '1' ? 1.0f : 0.0f);
  }
}
