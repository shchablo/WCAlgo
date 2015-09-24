#include "Weight.hh"

/*! \fn Weight
* \brief   constructtor
*/
Weight::Weight()
{
}

/*! \fn ~Weight
* \brief  destructor
*/
Weight::~Weight()
{

}

/*! \brief   Convert float array in double array
*
*   \param   int length (length array)
*   \param   float* data (array float)
*   \param   double* out (array double)
*
*   \return  double* out
*
*/
void Weight::float2double(int length, float* data, double* out)
{
    for(int i = 0; i < length; i++)
       out[i] = (double)data[i];
}

void Weight::middleSignal(int ch, vector<float> *data, vector<float> *time,
                            float *midData, float *midTime,
                            unsigned short int midLength, unsigned long int firstNumEvent, unsigned long int lastNumEven)
{
  for(unsigned short int i = 0; i < midLength; i++) {
    midTime[i] += time[0][i]/(lastNumEven - firstNumEvent);
    midData[i] += data[ch][i]/(lastNumEven - firstNumEvent);
  }
}

bool Weight::findWeight(float degree, float *data, float *weight, unsigned short int length)
{
  float max = numeric_limits<float>::min();
    for(int i = 0; i < length; i++) {
        if(max <= abs(data[i]))
          max = abs(data[i]);
    }
    for(int i = 0; i < length; i++)
        weight[i] = pow(abs(data[i])/max, degree);
  return true;
}

/*! \brief   loop function for analysis
*
* \return bool - check tag
*
*/
bool Weight::loop(int channel, float degree, unsigned long int firstNumEvent, unsigned long int lastNumEvent, char *weightDataName)
{
  unsigned long int numEvent = firstNumEvent;
  vector<float> data[4];
  vector<float> time;
  bool isRead = readDrsData(data, &time, numEvent);
  if(!isRead)
    return false;
  for(unsigned short int i = 0; i < 4; i++)
    data[i].clear();

  // middle signal
  unsigned short int midLength = time.size();
  time.clear();
  float midData[midLength];
  float midTime[midLength];
  for(unsigned short int i = 0; i < midLength; i++) {
    midTime[i] = 0;
    midData[i] = 0;
  }

  // loop
  cout << "# Processing:" << endl;
  cout << "--------------------------------------------------------------------------------";
  while(!feof(inFile_) && (numEvent < lastNumEvent)) {
    bool isRead = readDrsData(data, &time, numEvent);
    if(!isRead)
      return false;
    middleSignal(channel, data, &time, midData, midTime, midLength, firstNumEvent, lastNumEvent);
    for(unsigned short int i = 0; i < 4; i++)
      data[i].clear();
    time.clear();
    numEvent++;
    if(numEvent%((lastNumEvent - firstNumEvent)/80*1) == 0) {
      cout << "\r";
      for(int i = 0; i < numEvent/((lastNumEvent - firstNumEvent)/80*1); i++)
        cout << "#";
      for(int i = numEvent/((lastNumEvent - firstNumEvent)/80*1); i < 80; i++)
        cout << "-";
    }
  }
  cout << endl;

  float weight[midLength];
  findWeight(degree, midData, weight, midLength);
  TGraph *graphMidSignal = new TGraph(midLength, midTime, weight);
  graphMidSignal->SetName(weightDataName);
  graphMidSignal->SetTitle(weightDataName);
  graphMidSignal->GetXaxis()->SetTitle("time, ns");
  graphMidSignal->GetYaxis()->SetTitle("weight");
  writeObject("", graphMidSignal);
  graphMidSignal->Delete();

  return true;
}


