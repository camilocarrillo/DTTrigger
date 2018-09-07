/**
 * Project:
 * Subproject: tracrecons
 * File name:  datafilereader.h
 * Language:   C++
 *
 * *********************************************************************
 * Description:
 *
 *
 * To Do:
 *
 * Author: Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * *********************************************************************
 * Copyright (c) 2015-06-17 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "cexception.h"
#include "dtprimitive.h"
#include "globaldatapath.h"
#include "i_pusher.h"
#include "i_task.h"


using namespace CIEMAT;
using namespace std;

namespace CMS {

class DataFileReader : public I_Task {

  public:
    typedef struct {
      unsigned int totalLines;
    } STATISTICS;

    DataFileReader(I_Pusher &dataOut, GlobalDataPath &gdp,
                   string name = "", char tkSep = ',');
    virtual ~DataFileReader();

    void run(void) throw (CException);
    virtual void stop(void);
    void reset(void);
    bool isEndOfFile(void);

    string rightTrim(string value);
    string leftTrim(string value);

    void setFileName(string name);
    void setTokenSeparator(char separator);
    const STATISTICS* getStatistics(void);

  private:
    I_Pusher &dataOut;
    GlobalDataPath &gdp;
    string fileName;

    char tokenSeparator;
    bool active;
    bool endFile;

    STATISTICS stats;

    void parseLine(string line,  int *superLayer, int *layer, 
                   int *channel, int *tdcTStamp,  int *orbit, 
                   double *payloads)
      throw(CException);

    bool thereIsANotNullNumber(string str);
};

}

#endif
