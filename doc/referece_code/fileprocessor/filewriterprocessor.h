/**
 * Project:
 * Subproject: tracrecons
 * File name:  filewriterprocessor.h
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
 * Copyright (c) 2015-06-29 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef FILEWRITERPROCESSOR_H
#define FILEWRITERPROCESSOR_H

#include "datafilewriter.h"
#include "thread.h"
#include <string>

using namespace std;

namespace CMS {

class FileWriterProcessor : public DataFileWriter {

  public:
    FileWriterProcessor(I_Popper &dataIn, string name = "",
                        string tkSep = ", \t");
    virtual ~FileWriterProcessor();

    void start(void);
    void stop(void);
    bool isActive(void);

  private:
    Thread *thWriter;
};

}

#endif
