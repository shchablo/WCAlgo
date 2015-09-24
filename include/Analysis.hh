//-------------------------------------------------------------
#ifndef ANALYSIS_h
#define ANALYSIS_h

#include <WCAlgo.hh>
#include <DrsRead.hh>
#include <OutFileRoot.hh>
#include <DrawRoot.hh>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TGraph.h>
#include <TAxis.h>

// C++ includes
#include <iostream>
#include <math.h>

using namespace std;

class Analysis : public WCAlgo, public DrsRead, public OutFileRoot, public DrawRoot
{
public:

  //! constructor
  Analysis();

  //! destructor
  virtual ~Analysis();

  bool loop(int channel, unsigned long int firstNumEvent, unsigned long int lastNumEvent,
            float firstGate, float lastGate,
            char* weightFileName);

protected:
  vector<TGraph*> getWeightFun(char* weightFileName);
  bool setWeight(vector<TGraph*> graphs,
                           double **weight, double **time,
                           float firstGate, float lastGate, int *gate1, int *gate2, unsigned short int length);
  void float2double(int length, float* data, double* out);
  void middleSignal(int channel, vector<float> *data, vector<float> *time,
                    float *midData, float *midTime,
                    unsigned short int midLength,
                    unsigned long int firstNumEvent, unsigned long int lastNumEven);


};

#endif
