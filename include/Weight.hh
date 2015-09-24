//-------------------------------------------------------------
#ifndef WEIGHT_h
#define WEIGHT_h

#include <WCAlgo.hh>
#include <DrsRead.hh>
#include <OutFileRoot.hh>
#include <DrawRoot.hh>

// ROOT includes
#include <TGraph.h>
#include <TAxis.h>

// C++ includes
#include <iostream>
#include <math.h>

using namespace std;

class Weight : public WCAlgo, public DrsRead, public OutFileRoot, public DrawRoot
{
public:

  //! constructor
  Weight();

  //! destructor
  virtual ~Weight();

  bool loop(int channel, float degree, unsigned long int firstNumEvent, unsigned long int lastNumEvent, char *weightDataName);

protected:
  bool findWeight(float degree, float *data, float *weight, unsigned short int length);
  void float2double(int length, float* data, double* out);
  void middleSignal(int channel, vector<float> *data, vector<float> *time,
                    float *midData, float *midTime,
                    unsigned short int midLength,
                    unsigned long int firstNumEvent, unsigned long int lastNumEvenz);

};

#endif
