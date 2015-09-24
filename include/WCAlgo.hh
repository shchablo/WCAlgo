//-------------------------------------------------------------
#ifndef WCALGO_h
#define WCALGO_h


// C++ includes
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

class WCAlgo
{
public:

  //! constructor
  WCAlgo();

  //! destructor
  virtual ~WCAlgo();
 ;

  bool justIntegral(int gate1, int gate2, int channel,
                            vector<float> *data, float *integral,
                            int sign);
  bool weightIntegral(int channel, vector<float> *data, double *weight, float *integral, int sign, unsigned short int length);

private:

};

#endif
