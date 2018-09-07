/**
 * Project:
 * Subproject: tracrecons
 * File name:  datafilewriter.h
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
#ifndef DATAFILEWRITER_H
#define DATAFILEWRITER_H

#include <string>
#include "i_task.h"
#include "i_popper.h"
#include "muonpath.h"
#include <fstream>

using namespace CIEMAT;
using namespace std;

namespace CMS {

class DataFileWriter : public I_Task {

  public:
    DataFileWriter(I_Popper &inMuonPath, string name = "",
                   string tkSep = ", \t");
    virtual ~DataFileWriter();

    void run(void);
    void procesar(ofstream &file);
    void stop(void);

    void setFileName(string name);
    void setRRobin(bool rr);
    void setTokenSeparator(string separator);
    void setNumLinesBufferSize(int numLines = 10000);

  private:
    I_Popper &inMuonPath;
    string fileName;

    bool active;
    string tokenSeparator;
    bool roundRobin;
    int writeAfterNumLines;
    string lineBuffer;
    int lineCounter;

    string buildTextLine(MuonPath* const mPath);
    string buildTextLineForTest(MuonPath* const mPath);
    string addSeparator(string line);
};

}

#endif
