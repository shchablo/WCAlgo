#include "WCAlgo.hh"

/*! \fn WCAlgo
* \brief   constructtor
*/
WCAlgo::WCAlgo()
{

}

/*! \fn ~WCAlgo
* \brief  destructor
*/
WCAlgo::~WCAlgo()
{
}

bool WCAlgo::justIntegral(int gate1, int gate2, int channel,
                          vector<float> *data, float *integral,
                          int sign)
{
    *integral = 0;
    for(int i = gate1; i < gate2; i++) {
      if(data[channel][i] != 0)
        *integral += sign*data[channel][i];
    }
  return true;
}

bool WCAlgo::weightIntegral(int channel, vector<float> *data, double *weight, float *integral, int sign, unsigned short int length)
{
    *integral = 0;
    for(int i = 0; i < length; i++)
        *integral += sign*data[channel][i]*weight[i];
  return true;
}
