//-------------------------------------------------------
// Description: main file for analysis weigth convolution algorithm for ADC.
// Authors:  Shchablo, Shchablo@gmail.com
//-------------------------------------------------------

// C++ includes
#include <iostream>
#include <fstream>
#include <stdio.h>

// Analysis class
#include "Analysis.hh"
#include "Weight.hh"

using namespace std;

int main(int argc, char* argv[]) 
{
  cout <<"#------------------------------------------------------------------------------#" << endl;
  cout <<"#------------------------------------------------------------------------------#" << endl;
  cout <<"#                WCAlgo, Analysis weigth convolution algorithm                 #" << endl;
  cout <<"#------------------------------------------------------------------------------#" << endl;
  if (argc == 1) {
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# OPTIONS:                                                                      " << endl;
    cout << "# --weight          Start weight calculation                                    " << endl;
    cout << "# --outTree=[name]  Choose name of tree for output file                         " << endl;
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# Analysis data (default):                                                      " << endl;
    cout << "# Example: ./wcAlgo weight.root DRSfile output.root                             " << endl;
    cout << " -ch=[value] -fE=[value] -lE=[value] -fG=[value] -lG=[value]                    " << endl;
    cout << "# -ch=[value] Choose number of channel for analysis from DRS file               " << endl;
    cout << "# -fE=[value] Choose first event for read                                       " << endl;
    cout << "# -lE=[value] Choose last event for read                                        " << endl;
    cout << "# -fG=[value] Choose firs value (time form wight function) for gate [ns]        " << endl;
    cout << "# -lG=[value] Choose last value (time form wight function) for gate [ns]        " << endl;
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# Weight calculation (--weight):                                                " << endl;
    cout << "# Example: ./wcAlgo DRSfile weight.root --weight                                " << endl;
    cout << " -ch=[value] -fE=[value] -lE=[value] -[type]                                    " << endl;
    cout << "# --w=[name]  Choose name for weights data (default name = weights)             " << endl;
    cout << "# -ch=[value]  Choose number of channel from DRS file                           " << endl;
    cout << "# -fE=[value]  Choose first event for read                                      " << endl;
    cout << "# -lE=[value]  Choose last event for read                                       " << endl;
    cout << "# -[type]      Choose type of weight function type:                             " << endl;
    cout << "                                                   pf - power function          " << endl;
    cout << "# --deg=[value] Choose degree for power function                                " << endl;
    cout << "#------------------------------------------------------------------------------#" << endl;

    return 0;
  }
  char inputFileName[128] = "none";
  char weightFileName[128] = "none";
  char weightDataName[128] = "none";
  char outputFileName[128] = "none";
  char outputTreeName[128] = "default";

  int ch = 0;
  unsigned long int firstNumEvent = 0;
  unsigned long int lastNumEvent = 0;
  float firstGate = 0;
  float lastGate = 0;

  bool isWeight = false;
  float degree = 1;
  int type = 0;

  for(int i = 1; i < argc; i++) {
    if(strncmp(argv[i],"--weight", 6) == 0)
      isWeight = true;
  }
  if(!isWeight) {
  strcpy(weightFileName, argv[1]);
  strcpy(inputFileName, argv[2]);
  strcpy(outputFileName, argv[3]);
  for(int i = 1; i < argc; i++) {  
    if(strncmp(argv[i],"--outTree=", 7) == 0)
      sscanf(argv[i], "--outTree=%s", outputTreeName);
    if(strncmp(argv[i], "-fE=", 4) == 0)
      sscanf(argv[i], "-fE=%lu", &firstNumEvent);
    if(strncmp(argv[i], "-lE=", 4) == 0)
      sscanf(argv[i], "-lE=%lu", &lastNumEvent);
    if(strncmp(argv[i], "-fG=", 4) == 0)
      sscanf(argv[i], "-fG=%f", &firstGate);
    if(strncmp(argv[i], "-lG=", 4) == 0)
      sscanf(argv[i], "-lG=%f", &lastGate);
    if(strncmp(argv[i], "-ch=", 4) == 0)
      sscanf(argv[i], "-ch=%d", &ch);
  }
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# Analysis data:                                                                " << endl;
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# InputFile: " << inputFileName << endl;
    cout << "# firstEvent=" << firstNumEvent << " lastEvent=" << lastNumEvent << endl;
    cout << "# GateFile: " << inputFileName << endl;
    cout << "# OutputFile: " << outputFileName << endl;
    cout << "# OutputTree: " << outputTreeName << endl;
    cout << "# firstGate=" << firstGate << " [ns]" << " lastGate=" << lastGate << " [ns]" << endl;

    Analysis analysis;
    analysis.setInputFile(inputFileName);
    analysis.setOutputFile(outputFileName, outputTreeName);
    if(firstNumEvent >= 0 && lastNumEvent > 0)
      analysis.loop(ch, firstNumEvent, lastNumEvent, firstGate, lastGate, weightFileName);
    else
      cout << "# Can't run loop function, for help execute ./wcAlgo" << endl;
  }
  else {
    strcpy(inputFileName, argv[1]);
    strcpy(outputFileName, argv[2]);
    for(int i = 1; i < argc; i++) {
      if(strncmp(argv[i],"--outTree=", 7) == 0)
        sscanf(argv[i], "--outTree=%s", outputTreeName);
      if(strncmp(argv[i],"--w=", 4) == 0)
        sscanf(argv[i], "--w=%s", weightDataName);
      if(strncmp(argv[i], "-fE=", 4) == 0)
        sscanf(argv[i], "-fE=%lu", &firstNumEvent);
      if(strncmp(argv[i], "-lE=", 4) == 0)
        sscanf(argv[i], "-lE=%lu", &lastNumEvent);
      if(strncmp(argv[i], "-ch=", 4) == 0)
        sscanf(argv[i], "-ch=%d", &ch);
      if(strncmp(argv[i], "-pf", 5) == 0)
        type = 0;
      if(strncmp(argv[i], "--deg=", 6) == 0)
        sscanf(argv[i], "--deg=%f", &degree);
    }
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# Weights calculation:                                                         #" << endl;
    cout << "#------------------------------------------------------------------------------#" << endl;
    cout << "# type: ";
    if(type == 0)
      cout << "Power function with degree=" << degree << endl;
    cout << "# InputFile: " << inputFileName << endl;
    cout << "# firstEvent=" << firstNumEvent << " lastEvent=" << lastNumEvent << endl;
    cout << "# OutputFile: " << outputFileName << endl;

    Weight weight;
    weight.setInputFile(inputFileName);
    weight.setOutputFile(outputFileName, outputTreeName);
    if(firstNumEvent >= 0 && lastNumEvent > 0) {
      if(type == 0)
        weight.loop(ch, degree, firstNumEvent, lastNumEvent, weightDataName);
    }
    else
      cout << "# Can't run loop function, for help execute ./wcAlgo" << endl;
  }

  cout << "# The End" << endl;
  cout << "#------------------------------------------------------------------------------#" << endl;
  return 1;
}
