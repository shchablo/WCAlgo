#include "DrsRead.hh"

/*! \fn DrsRead
* \brief   constructtor
*/
DrsRead::DrsRead()
{
  isInFile_ = false;
}

/*! \fn ~DrsRead
* \brief  destructor
*/
DrsRead::~DrsRead()
{
  if(isInFile_)
    fclose(inFile_);
}

/*! \fn setInputFile
* \brief Function set input file.
*
* \param  char* inputFileName - path to the file
* \return bool - check tag
*/
bool DrsRead::setInputFile(char *inputFileName)
{
  inFile_ = fopen(inputFileName, "rb");
  if(!inFile_)
    return false;
  isInFile_ = true;
  return true;
}

/*! \fn readDrsData
* \brief Function for upload input data
* (don't read header, only for 1024 words per frame)
*
* \param  vector<float> *data - array for voltage data [V]
* \param  vector<float> *time - array for time data
*
* unsigned long int numEvent - number of event for read
* \return bool - check tag
*/
bool DrsRead::readDrsData(vector<float> *data, vector<float> *time, unsigned long int numEvent)
{
  if(!isInFile_)
    return false;
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

  unsigned short int iBlockSize16 = 1024; //Number of read words per frame (This is for Vytautas SoftWare) Number of bins
  unsigned short int iBlockVoltageArray[iBlockSize16]; //Array for Voltage Ch1
  float fBlockTimeArray[iBlockSize16]; //Array for Time Scale
  for(unsigned short int i = 0; i < iBlockSize16; i++) {
    iBlockVoltageArray[i] = 0;
    for(unsigned short int j = 0; j < 4; j++)
      data[j].push_back(float(iBlockVoltageArray[i]));
    fBlockTimeArray[i] = 0;
    time->push_back(fBlockTimeArray[i]);
  }
  unsigned short int ch = 0;
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

  fseek(inFile_, numEvent*size, SEEK_SET);
  fread(&Header, 4, 1, inFile_);

  if(Header == 0x52444845) { //"EHDR" Checking
    fread(&SerNum, 4, 1, inFile_);
    fread(&Year, 2, 1, inFile_); fread(&Month, 2, 1, inFile_); fread(&Day, 2, 1, inFile_);
    fread(&Hour, 2, 1, inFile_); fread(&Minute, 2, 1, inFile_); fread(&Second, 2, 1, inFile_);
    fread(&Millisecond, 2, 1, inFile_); fread(&Reserved, 2, 1, inFile_);

    fread(&fBlockTimeArray, 4, iBlockSize16, inFile_);
    for(unsigned short int i = 0; i < iBlockSize16; i++)
      time[0][i] = fBlockTimeArray[i];
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
