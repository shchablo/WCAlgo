//-------------------------------------------------------------
#ifndef DRSREAD_h
#define DRSREAD_h

// C++ includes
#include <vector>
#include <iostream>

using namespace std;

class DrsRead
{
public:

  //! constructor
  DrsRead();

  //! destructor
  virtual ~DrsRead();

  bool setInputFile(char *inputFileName);
  bool readDrsData(vector<float> *data, vector<float> *time, unsigned long int numEvent);

protected:

  bool isInFile_;
  FILE  *inFile_;
};

#endif
