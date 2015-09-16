//-------------------------------------------------------
// Description: main file for analysis algorithms
// Authors:  Shchablo, Shchablo@gmail.com
//-------------------------------------------------------

// C++ includes
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

// Analyses class
#include "WCAlgo.hh"

using namespace std;

int main(int argc, char* argv[]) 
{
  cout <<"#------------------------------------------------------------" << endl;
  cout <<"#------------------------------------------------------------" << endl;
  cout <<"#       WCAlgo, Analysis weigth convolution algorithm"         << endl;
  cout <<"#------------------------------------------------------------" << endl;
  if ( argc < 2 ) {
    cout <<"#------------------------------------------------------------" << endl;
    cout << "To run WCAlgo app please specify the input file and output file" << endl; 
    cout << "Example: ./jetAlgo -input=DRSfile -output=outputFile.root"  << endl;
    cout << "OPTIONS:"                                                   << endl;
    cout << "-input=[name]  Name of the input file"                      << endl;
    cout << "-output=[name] Name of the output file"                     << endl;
    cout << "--tree=[name]  Choose name of tree for output file"         << endl;
    
    cout << "--gate1=[value]      Choose firs value for gate"         << endl;
    cout << "--gate2=[value]      Choose last value for gate"         << endl;
    cout << "--numEvent1=[value]  Choose first number event for read"         << endl;
    cout << "--numEvent2=[value]  Choose last number event for read"         << endl;
    cout << "--analysis           Run analysis for algorithm"                  << endl;

    return 1;
  }

  char inputDRSFile[128] = "none";
  char outputFileName[128] = "none";
  char outputTreeName[128] = "default";
  float gate1 = 0; float gate2 = 0;
  float numEvent1 = 0; float numEvent2 = 0;
  float degree = 1;
  bool isAnalysis = false;
  for(int i = 1; i < argc; i++) {
    if(strncmp(argv[i], "-input=", 7) == 0)
      sscanf(argv[i], "-input=%s", inputDRSFile);
    if(strncmp(argv[i], "-output=", 8) == 0)
      sscanf(argv[i], "-output=%s", outputFileName);
    if(strncmp(argv[i],"--tree=", 7) == 0)
      sscanf(argv[i], "--tree=%s", outputTreeName);
    if(strncmp(argv[i], "--analysis", 10) == 0)
      isAnalysis = true;
    if(strncmp(argv[i], "--gate1=", 8) == 0)
      sscanf(argv[i], "--gate1=%f", &gate1);
    if(strncmp(argv[i], "--gate2=", 8) == 0)
      sscanf(argv[i], "--gate2=%f", &gate2);
    if(strncmp(argv[i], "--numEvent1=", 12) == 0)
      sscanf(argv[i], "--numEvent1=%f", &numEvent1);
    if(strncmp(argv[i], "--numEvent2=", 12) == 0)
      sscanf(argv[i], "--numEvent2=%f", &numEvent2);
    if(strncmp(argv[i], "--degree=", 9) == 0)
      sscanf(argv[i], "--degree=%f", &degree);
  }
  if(strncmp(outputFileName, "none", 4) == 0) { 
    cout << "Can't run program" << endl;
    return 0;
  }
  if(strncmp(inputDRSFile, "none", 4) == 0) {
    cout << "Set input file" << endl;
    return 0;
  }
    cout <<"#------------------------------------------------------------" << endl;
    cout << "# InputFile: " << inputDRSFile << endl;
    cout << "# OutputFile: " << outputFileName << endl;
    cout << "# OutputTree: " << outputTreeName << endl;
    cout <<"#------------------------------------------------------------" << endl;
    WCAlgo wcAlgo;
    bool isInputFile = wcAlgo.setInputFile(inputDRSFile);
    if(!isInputFile) {
      cout << "Can't open input file" << endl;
      return 0;
    }
  
  if(strncmp(outputFileName, "none", 4) != 0) { 
  }
    bool isOutputFile = wcAlgo.setOutputFile(outputFileName, outputTreeName);
    if(!isOutputFile) {
      cout << "Can't create output file" << endl;
      return 0;
    }
    bool isLoop = wcAlgo.loop(numEvent1, numEvent2, gate1, gate2, degree);
    if(!isLoop) {
      cout << "Error in loop middle signal fun" << endl;
      return 0;
    }
  cout << "The End" << endl;
  return 1;
}
