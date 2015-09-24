#include "Analysis.hh"

/*! \fn Analysis
* \brief   constructtor
*/
Analysis::Analysis()
{

}

/*! \fn ~Analysis
* \brief  destructor
*/
Analysis::~Analysis()
{

}

vector<TGraph*> Analysis::getWeightFun(char* weightFileName)
{
  vector<TGraph*> graphs;

   TFile *weightFile = new TFile(weightFileName, "UPDATE");
  if(!weightFile)
    return graphs;

  TIter iter(weightFile->GetListOfKeys());
  iter.Begin();
  while(iter != iter.End()) {
    if(dynamic_cast<TGraph*>((TObject*)weightFile->Get((*iter)->GetName())) != 0)
      graphs.push_back((TGraph*)weightFile->Get((*iter)->GetName()));
  iter.Next();
  }
  weightFile->Close();
  weightFile->Delete();
  return graphs;
}
/*! \fn setWeightFile
* \brief Function for upload input data.
*
* \param  char* inputFile - path to the file
* \return bool - check tag
*/
bool Analysis::setWeight(vector<TGraph*> graphs,
                         double **weight, double **time,
                         float firstGate, float lastGate, int *gate1, int *gate2, unsigned short int length)
{

  for(int n = 0; n < graphs.size(); n++) {
    weight[n] = new double[length];
    time[n] = new double[length];
    for(int i = 0; i < length; i++) {
      graphs[n]->GetPoint(i, time[n][i], weight[n][i]);
      if(time[n][i] < firstGate) *gate1 = i;
      if(time[n][i] < lastGate) *gate2 = i;
    }
      for(int i = 0; i < length; i++) {
        if(i < *gate1 || i > *gate2) {
          graphs[n]->GetPoint(i, time[n][i], weight[n][i]);
          weight[n][i] = 0;
        }
        else
          graphs[n]->GetPoint(i, time[n][i], weight[n][i]);
      }
    }
  return true;
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
void Analysis::float2double(int length, float* data, double* out)
{
    for(int i = 0; i < length; i++)
       out[i] = (double)data[i];
}

void Analysis::middleSignal(int channel, vector<float> *data, vector<float> *time,
                            float *midData, float *midTime,
                            unsigned short int midLength, unsigned long int firstNumEvent, unsigned long int lastNumEven)
{
  for(unsigned short int i = 0; i < midLength; i++) {
    midTime[i] += time[0][i]/(lastNumEven - firstNumEvent);
    midData[i] += data[channel][i]/(lastNumEven - firstNumEvent);
  }
}


/*! \brief   loop function for analysis
*
* \return bool - check tag
*
*/
bool Analysis::loop(int channel, unsigned long int firstNumEvent, unsigned long int lastNumEvent,
                    float firstGate, float lastGate,
                    char* weightFileName)
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
  vector<TGraph*> graphs;
  graphs = getWeightFun(weightFileName);
  double *weight[graphs.size()];
  double *timeWeight[graphs.size()];
  int gate1;
  int gate2;
  setWeight(graphs, weight, timeWeight, firstGate, lastGate, &gate1, &gate2, midLength);
  cout << "# firstGate=" << gate1 << " [point]" << " lastGate=" << gate2 << " [point]" << endl;

  float integral = 0;
  TH1F *h = new TH1F (Form("integral_without_weight"), Form("integral_without_weight"), 300, 1, -1);
  h->GetXaxis()->SetTitle("integral");
  h->GetYaxis()->SetTitle("Counts");
  h->GetYaxis()->SetTitleOffset(1.6);
  float weightsIntegrals[graphs.size()];
  TH1F *wh[graphs.size()];
  for(unsigned short int i = 0; i < graphs.size(); i++) {
    wh[i] = new TH1F (Form("integral_%s", graphs[i]->GetName()), Form("integral_%s", graphs[i]->GetName()), 300, 1, -1);
    wh[i]->GetXaxis()->SetTitle("integral");
    wh[i]->GetYaxis()->SetTitle("Counts");
    wh[i]->GetYaxis()->SetTitleOffset(1.6);
    weightsIntegrals[i] = 0;
  }

  // loop
  cout << "# Processing:" << endl;
  cout << "--------------------------------------------------------------------------------";
  while(!feof(inFile_) && (numEvent < lastNumEvent)) {
    bool isRead = readDrsData(data, &time, numEvent);
    if(!isRead)
      return false;
    middleSignal(channel, data, &time, midData, midTime, midLength, firstNumEvent, lastNumEvent);

    justIntegral(gate1, gate2, channel, data, &integral, -1);
    h->Fill(integral);
     for(unsigned short int i = 0; i < graphs.size(); i++) {
         weightIntegral(channel, data, weight[i], &weightsIntegrals[i], -1, midLength);
         wh[i]->Fill(weightsIntegrals[i]);
     }

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

  TGraph *graphMidSignal = new TGraph(midLength, midTime, midData);
  graphMidSignal->SetName("middle_signal");
  graphMidSignal->SetTitle("middle_signal");
  graphMidSignal->GetXaxis()->SetTitle("time, ns");
  graphMidSignal->GetYaxis()->SetTitle("amplitude, V");
  writeObject("", graphMidSignal);
  graphMidSignal->Delete();

  TGraph *graphWeight[graphs.size()];
  for(unsigned short int n = 0; n < graphs.size(); n++) {
    graphWeight[n] = new TGraph(midLength, timeWeight[n], weight[n]);
    graphWeight[n]->SetName(graphs[n]->GetName());
    graphWeight[n]->SetTitle(graphs[n]->GetName());
    graphWeight[n]->GetXaxis()->SetTitle("time, ns");
    graphWeight[n]->GetYaxis()->SetTitle("weight");
    writeObject("", graphWeight[n]);
    graphWeight[n]->Delete();
  }
  writeObject("", h);
  h->Delete();
  for(unsigned short int n = 0; n < graphs.size(); n++) {
    writeObject("", wh[n]);
    wh[n]->Delete();
  }
  return true;
}


