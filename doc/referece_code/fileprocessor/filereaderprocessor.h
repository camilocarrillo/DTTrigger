/**
 * Project:
 * Subproject: tracrecons
 * File name:  filereaderprocessor.h
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
 * Copyright (c) 2015-06-18 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef FILEREADERPROCESSOR_H
#define FILEREADERPROCESSOR_H

#include "datafilereader.h"
#include "globaldatapath.h"
#include "thread.h"
#include <string>

using namespace std;

namespace CMS {

class FileReaderProcessor : public DataFileReader {

  public:
    FileReaderProcessor(I_Pusher &dataOut, GlobalDataPath &gdp, 
                        string name = "", char tkSep = ',');
    virtual ~FileReaderProcessor();

    void start(void);
    void stop(void);
    bool isActive(void);

  private:
    Thread *thReader;
};

}

#endif
