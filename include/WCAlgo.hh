//-------------------------------------------------------------
#ifndef WCAlgo_h
#define WCAlgo_h

// C++ includes
#include <string>
#include <math.h>
#include <iostream>
#include <vector>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TDirectory.h>
#include <TObject.h>
#include <TAxis.h>
#include <TH1F.h>

using namespace std;

class WCAlgo
{
public:

  //! constructor
  WCAlgo();

  //! destructor
  virtual ~WCAlgo();

  bool setInputFile(char*);
  bool setOutputFile(char*, char*);
  bool setBranch(float*, string); 

  bool weights(float, float, float, vector<float>*, vector<float>*, vector<float>*); 
  bool justIntegral(int, int, vector<float>*, vector<float>*, float*); 
  bool weightIntegral(int, int, vector<float>*, vector<float>*, vector<float>*, float*); 
  
  //! loop over events (run algorithm)
  bool loopMiddleSignal(int, int);
  bool loopWeightMiddleSignal(int, int, float, float, float, vector<float>*);
  bool loopIntegral(int, int, float, float); 
  bool loopWeightIntegral(int, int, float, float, vector<float>*); 
  bool loop(int, int, float, float, float); 

private:

  bool isInFile_;
  FILE  *inFile_;
  
  bool isOutFile_;
  TFile *outFile_;
  TTree *outTree_;
  
  bool writeGraph(string, TGraph*);
  bool writeHist(string, TH1F*);
  bool readDrsData(vector<float>*, vector<float>*, int);
 
};

#endif
