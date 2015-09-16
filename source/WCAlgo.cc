#include "WCAlgo.hh"

/*! \fn WCAlgo    
* \brief   constructtor
*/
WCAlgo::WCAlgo() 
{
  isInFile_ = false;
  isOutFile_ = false;
}

/*! \fn ~jetAlgo     
* \brief  destructor
*/
WCAlgo::~WCAlgo() 
{
  if(isInFile_)
    fclose(inFile_);
  
  if(isOutFile_) {
    outTree_->Delete();
    outFile_->Close();
    outFile_->Delete();
  }
}

/*! \fn setInputFile     
* \brief Function for upload input data.
*
* \param  char* inputFile - path to the file
* \return bool - check tag 
*/
bool WCAlgo::setInputFile(char* inputFileName) 
{
  inFile_ = fopen(inputFileName, "rb");
  if(!inFile_) 
    return false;
  isInFile_ = true;
  return true;
}

/*! \fn setInputFile     
* \brief Function for upload input data.
*
* \param  char* inputFile - path to the file
* \return bool - check tag 
*/
bool WCAlgo::setOutputFile(char* outputFileName, char* outputTreeName) 
{
  outFile_ = new TFile(outputFileName, "UPDATE");
  if(!outFile_)
    return false;
  outTree_ = new TTree(outputTreeName, outputTreeName);
  if(!outTree_)
    return false;
  isOutFile_ = true;
  return true;
}

/*! \fn setInputFile     
* \brief Function for upload input data.
*
* \param  char* inputFile - path to the file
* \return bool - check tag 
*/
bool WCAlgo::setBranch(float *data, string branchName) 
{
  if(!isOutFile_)
    return false;
  outTree_->Branch(branchName.c_str(), data, branchName.c_str());
  return true;
}

/*! \fn readDrsData     
* \brief Function for upload input data.
*
* \param  char* inputFile - path to the file
* \return bool - check tag 
*/
bool WCAlgo::readDrsData(vector<float> *data, vector<float> *dataTime, int numberEvent) 
{
  fseek(inFile_, 0, SEEK_SET);
  
  int Header = 0x52444845; //"EHDR"
  
  int Channel; 
  int Channels[4]; 
  Channels[0] = 0x31303043; 
  Channels[1] = 0x32303043; 
  Channels[2] = 0x33303043; 
  Channels[3] = 0x34303043; 
  
  int SerNum = 0;
  short int Year = 0, Month = 0, Day = 0; 
  short int Hour = 0, Minute = 0, Second = 0; 
  short int Millisecond = 0, Reserved = 0; 
  
  int iBlockSize16 = 1024; //Number of read words per frame (This is for Vytautas SoftWare) Number of bins
  unsigned short int iBlockVoltageArray[iBlockSize16]; //Array for Voltage Ch1
  float fBlockTimeArray[iBlockSize16]; //Array for Time Scale
  for(int i = 0; i < iBlockSize16; i++) {
    iBlockVoltageArray[i] = 0;
    for(int j = 0; j < 4; j++)
      data[j].push_back(float(iBlockVoltageArray[i]));
    fBlockTimeArray[i] = 0;
    dataTime->push_back(fBlockTimeArray[i]);
  }
  int ch = 0; 
  fread(&Header, 4, 1, inFile_);
  if(Header == 0x52444845) { //"EHDR" Checking
    fread(&SerNum, 4, 1, inFile_);
    fread(&Year, 2, 1, inFile_); fread(&Month, 2, 1, inFile_); fread(&Day, 2, 1, inFile_);
    fread(&Hour, 2, 1, inFile_); fread(&Minute, 2, 1, inFile_); fread(&Second, 2, 1, inFile_);
    fread(&Millisecond, 2, 1, inFile_); fread(&Reserved, 2, 1, inFile_);
    
    fread(&fBlockTimeArray, 4, iBlockSize16, inFile_);
    for(unsigned short int i = 0; i < 4; i++) {
      fread(&Channel, 4, 1, inFile_);
      if(Channel == Channels[i]) {
        fread(&iBlockVoltageArray[0], 2, iBlockSize16, inFile_);
        ch++;
      }
    }
  }
  else
    return false;

  fseek(inFile_, 0, SEEK_SET);
  size_t size = sizeof(Header)+ch*sizeof(Channel)+sizeof(SerNum)+
                sizeof(Year)+sizeof(Month)+sizeof(Day)+
                sizeof(Hour)+sizeof(Minute)+sizeof(Second)+
                sizeof(Millisecond)+sizeof(Reserved)+
                sizeof(fBlockTimeArray)+ch*sizeof(iBlockVoltageArray);
  
  fseek(inFile_, numberEvent*size, SEEK_SET);
  fread(&Header, 4, 1, inFile_);

  if(Header == 0x52444845) { //"EHDR" Checking
    fread(&SerNum, 4, 1, inFile_);
    fread(&Year, 2, 1, inFile_); fread(&Month, 2, 1, inFile_); fread(&Day, 2, 1, inFile_);
    fread(&Hour, 2, 1, inFile_); fread(&Minute, 2, 1, inFile_); fread(&Second, 2, 1, inFile_);
    fread(&Millisecond, 2, 1, inFile_); fread(&Reserved, 2, 1, inFile_);
    
    fread(&fBlockTimeArray, 4, iBlockSize16, inFile_);
    for(unsigned short int i = 0; i < iBlockSize16; i++)
      dataTime[0][i] = fBlockTimeArray[i];
    for(unsigned short int i = 0; i < 4; i++) {
      fread(&Channel, 4, 1, inFile_);
      if(Channel == Channels[i]) {
        fread(&iBlockVoltageArray[0], 2, iBlockSize16, inFile_);
        for(unsigned short int j = 0; j < iBlockSize16; j++) {
          data[i][j] = float(iBlockVoltageArray[j]-32768)/65536; // Voltage [V]
        }
      }
    }
  }
  else
    return false;
  return true;
}

bool WCAlgo::writeGraph(string dirName, TGraph *graph) 
{
  if(!outFile_->GetDirectory(dirName.c_str())) {
    outFile_->mkdir(dirName.c_str());
    outFile_->cd(dirName.c_str());
  }
  else
    outFile_->cd(dirName.c_str());

//    if(outFile_->FindObject(graph->GetName()))
//     return false; 

  graph->Write();
  return true;
}

bool WCAlgo::writeHist(string dirName, TH1F *hist) 
{
  if(!outFile_->GetDirectory(dirName.c_str())) {
    outFile_->mkdir(dirName.c_str());
    outFile_->cd(dirName.c_str());
  }
  else
    outFile_->cd(dirName.c_str());

//    if(outFile_->FindObject(graph->GetName()))
//     return false; 

  hist->Write();
  return true;
}

bool WCAlgo::weights(float firstGate, float lastGate, float degree, vector<float> *data, vector<float> *time, vector<float> *weight) 
{
  float max[4]; 
  int gate1 = 0; int gate2 = 0;
  for(int i = 0; i < time->size(); i++) {
    if(time[0][i] < firstGate) gate1 = i;
    if(time[0][i] < lastGate) gate2 = i;
  }
  float step = (lastGate - firstGate)/(gate2 - gate1);
  for(unsigned short int i = 0; i < 4; i++) {
    weight[i].resize(time->size());
    max[i] = numeric_limits<float>::min(); 
    for(int j = gate1; j < gate2; j++) { 
        if(max[i] <= abs(data[i][j]))
          max[i] = abs(data[i][j]);
    }
    for(int j = 0; j < time->size(); j++) { 
      if(j < gate1 || j > gate2)
        weight[i][j] = 0;
      else
        weight[i][j] = pow(abs(data[i][j])/max[i], degree);
    }
  }
}

bool WCAlgo::justIntegral(int firstGate, int lastGate, 
                          vector<float> *data, vector<float> *time,
                          float *integral) 
{
  int gate1 = 0; int gate2 = 0;
  for(int i = 0; i < time->size(); i++) {
    if(time[0][i] < firstGate) gate1 = i;
    if(time[0][i] < lastGate) gate2 = i;
  }
  for(unsigned short int i = 0; i < 4; i++) {
    integral[i] = 0;
    for(int j = gate1; j < gate2; j++) { 
      if(data[i][j] != 0) 
        integral[i] += -data[i][j];
    }
  }
  return true;  
}

bool WCAlgo::weightIntegral(int firstGate, int lastGate, 
                      vector<float> *data, vector<float> *time, vector<float> *weight,
                      float *integral) 
{
  int gate1 = 0; int gate2 = 0;
  for(int i = 0; i < time->size(); i++) {
    if(time[0][i] < firstGate) gate1 = i;
    if(time[0][i] < lastGate) gate2 = i;
  }
  for(unsigned short int i = 0; i < 4; i++) {
    integral[i] = 0;
    for(int j = 0; j < time->size(); j++) { 
      if(data[i][j] != 0) 
        integral[i] += -data[i][j]*weight[i][j];
    }
  }
  return true;  
}

bool WCAlgo::loopMiddleSignal(int firstNumEvent, int lastNumEvent) 
{
  vector<float> data[4];
  vector<float> time;
  vector<float> gateData[4];
  vector<float> gateTime;

  int numberEvent = firstNumEvent;
  while(!feof(inFile_) && (numberEvent <= lastNumEvent)) {
    bool isRead = readDrsData(data, &time,  numberEvent);
    if(!isRead)
      break;
   if(numberEvent == firstNumEvent) { 
     for(unsigned short int i = 0; i < time.size(); i++) {
       for(unsigned short int j = 0; j < 4; j++) {
         gateData[j].push_back(data[j][i]);
       }
       gateTime.push_back(time[i]);
     }
   }
   else {
     for(unsigned short int i = 0; i < time.size(); i++) {
       for(unsigned short int j = 0; j < 4; j++)
         gateData[j][i] += data[j][i];
       gateTime[i] += time[i];
     }
   }
  for(unsigned short int i = 0; i < 4; i++)
    data[i].clear();
  time.clear();
  numberEvent++;
  }
   
  float dataArray[4][gateTime.size()];  
  float timeArray[gateTime.size()];  
  for(unsigned short int i = 0; i < gateTime.size(); i++) {
    for(unsigned short int j = 0; j < 4; j++)
      dataArray[j][i] = gateData[j][i]/(numberEvent-firstNumEvent + 1);
    timeArray[i] = gateTime[i]/(numberEvent-firstNumEvent + 1);
  }
  TGraph *graph[4];
  for(unsigned short int i = 0; i < 4; i++) {
    graph[i] = new TGraph(gateTime.size(), timeArray, dataArray[i]);
    graph[i]->SetName(Form("channel_%d", i));
    graph[i]->SetTitle(Form("channel_%d", i));
    graph[i]->GetXaxis()->SetTitle("Time, ns");
    graph[i]->GetYaxis()->SetTitle("Amplitude, V");
 
    float ch_check = 0;
    for(unsigned short int j = 0; j < gateTime.size(); j++)
      ch_check += dataArray[i][j];  
    if(ch_check == 0) { 
      graph[i]->SetTitle(Form("channel_%d_off", i));
      graph[i]->SetName(Form("channel_%d_off", i));
    }
    writeGraph("middleSignals", graph[i]);
    graph[i]->Delete();
  }
  return true;
}

bool WCAlgo::loopWeightMiddleSignal(int firstNumEvent, int lastNumEvent, float firstGate, float lastGate, float degree, vector<float> *weight) 
{
  vector<float> data[4];
  vector<float> time;
  vector<float> gateData[4];
  vector<float> gateTime;

  int numberEvent = firstNumEvent;
  while(!feof(inFile_) && (numberEvent <= lastNumEvent)) {
    bool isRead = readDrsData(data, &time,  numberEvent);
    if(!isRead)
      break;
   if(numberEvent == firstNumEvent) { 
     for(unsigned short int i = 0; i < time.size(); i++) {
       for(unsigned short int j = 0; j < 4; j++) {
         gateData[j].push_back(data[j][i]);
       }
       gateTime.push_back(time[i]);
     }
   }
   else {
     for(unsigned short int i = 0; i < time.size(); i++) {
       for(unsigned short int j = 0; j < 4; j++)
         gateData[j][i] += data[j][i];
       gateTime[i] += time[i];
     }
   }
  for(unsigned short int i = 0; i < 4; i++)
    data[i].clear();
  time.clear();
  numberEvent++;
  }
   
  float dataArray[4][gateTime.size()];  
  float timeArray[gateTime.size()];  
  for(unsigned short int i = 0; i < gateTime.size(); i++) {
    for(unsigned short int j = 0; j < 4; j++) {
      dataArray[j][i] = gateData[j][i]/(numberEvent-firstNumEvent + 1);
      gateData[j][i] = gateData[j][i]/(numberEvent-firstNumEvent + 1);
    }
    timeArray[i] = gateTime[i]/(numberEvent-firstNumEvent + 1);
    gateTime[i] = gateTime[i]/(numberEvent-firstNumEvent + 1);
  }
  
  weights(firstGate, lastGate, degree, gateData, &gateTime, weight); 
  float weightArray[4][gateTime.size()]; 
  for(unsigned short int i = 0; i < gateTime.size(); i++) {
    for(unsigned short int j = 0; j < 4; j++)
      weightArray[j][i] = weight[j][i];
  }
  
  TGraph *graph[4];
  for(unsigned short int i = 0; i < 4; i++) {
    graph[i] = new TGraph(gateTime.size(), timeArray, weightArray[i]);
    graph[i]->SetName(Form("channel_%d", i));
    graph[i]->SetTitle(Form("channel_%d", i));
    graph[i]->GetXaxis()->SetTitle("Time, ns");
    graph[i]->GetYaxis()->SetTitle(Form("weight_deg_%2.f",degree));
 
    float ch_check = 0;
    for(unsigned short int j = 0; j < gateTime.size(); j++)
      ch_check += dataArray[i][j];  
    if(ch_check == 0) { 
      graph[i]->SetTitle(Form("channel_%d_off", i));
      graph[i]->SetName(Form("channel_%d_off", i));
    }
    writeGraph(Form("weight_deg_%2.f", degree), graph[i]);
    graph[i]->Delete();
  }
  return true;
}

bool WCAlgo::loopIntegral(int firstNumEvent, int lastNumEvent, float firstGate, float lastGate) 
{
  TH1F *histo[4];
  for(unsigned short int i = 0; i < 4; i++) {
    histo[i] = new TH1F (Form("integral_ch%d", i),"Charge_Spectrum_integral", 300, 1, -1);
    histo[i]->GetXaxis()->SetTitle("integral, V*ns");
    histo[i]->GetYaxis()->SetTitle("Counts    ");
    histo[i]->GetYaxis()->SetTitleOffset(1.6);
  }
  vector<float> data[4];
  vector<float> time;
  float fIntegral[4];
  float fPed[4];
  
  int numberEvent = firstNumEvent;
  while(!feof(inFile_) && (numberEvent <= lastNumEvent)) {
    bool isRead = readDrsData(data, &time,  numberEvent);
    if(!isRead)
      break;
  justIntegral(firstGate, lastGate, data, &time, fIntegral);
  for(unsigned short int i = 0; i < 4; i++)
      histo[i]->Fill(fIntegral[i]);

  for(unsigned short int i = 0; i < 4; i++)
    data[i].clear();
  time.clear();
  numberEvent++;
  }
  for(unsigned short int i = 0; i < 4; i++)
    writeHist("integral", histo[i]);
  return true;
}

bool WCAlgo::loopWeightIntegral(int firstNumEvent, int lastNumEvent, float firstGate, float lastGate, vector<float> *weight) 
{
  TH1F *histo[4];
  for(unsigned short int i = 0; i < 4; i++) {
    histo[i] = new TH1F (Form("integral_ch%d", i),"Charge_Spectrum_integral", 300, 1, -1);
    histo[i]->GetXaxis()->SetTitle("integral, V*ns");
    histo[i]->GetYaxis()->SetTitle("Counts    ");
    histo[i]->GetYaxis()->SetTitleOffset(1.6);
  }
  vector<float> data[4];
  vector<float> time;
  float fIntegral[4];
  float fPed[4];
  
  int numberEvent = firstNumEvent;
  while(!feof(inFile_) && (numberEvent <= lastNumEvent)) {
    bool isRead = readDrsData(data, &time,  numberEvent);
    if(!isRead)
      break;
  weightIntegral(firstGate, lastGate, data, &time, weight, fIntegral);
  for(unsigned short int i = 0; i < 4; i++)
      histo[i]->Fill(fIntegral[i]);

  for(unsigned short int i = 0; i < 4; i++)
    data[i].clear();
  time.clear();
  numberEvent++;
  }
  for(unsigned short int i = 0; i < 4; i++)
    writeHist("weight_integral", histo[i]);
  return true;
}

bool WCAlgo::loop(int firstNumEvent, int lastNumEvent, 
                  float firstGate, float lastGate, float degree) 
{    
  bool isLoopMiddleSignal = loopMiddleSignal(firstNumEvent, lastNumEvent);
    if(!isLoopMiddleSignal)
      return false;
  
  vector<float> weight[4];
  bool isLoopWeightMiddleSignal = loopWeightMiddleSignal(firstNumEvent, lastNumEvent, firstGate, lastGate, degree, weight);
  if(!isLoopWeightMiddleSignal)
    return false;
  
  bool isLoopJustIntegral = loopIntegral(firstNumEvent, lastNumEvent, firstGate, lastGate);
  if(!isLoopJustIntegral)
    return false;
  
  bool isLoopWeightIntegral = loopWeightIntegral(firstNumEvent, lastNumEvent, firstGate, lastGate, weight);
  if(!isLoopWeightIntegral)
    return false;
  return true;
}

