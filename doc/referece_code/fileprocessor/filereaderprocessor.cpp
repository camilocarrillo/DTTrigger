/**
 * Project:
 * Subproject: tracrecons
 * File name:  filereaderprocessor.cpp
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
#include "filereaderprocessor.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
FileReaderProcessor::FileReaderProcessor(I_Pusher &dataOut, GlobalDataPath &gdp, 
                                         string name, char tkSep) :
  DataFileReader(dataOut, gdp, name, tkSep)
{

}

FileReaderProcessor::~FileReaderProcessor() { }

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void FileReaderProcessor::start(void) {
  thReader = new Thread(10);
  thReader->setCtrlJobExceptions(false);
  thReader->doThisJob(*this);
}

void FileReaderProcessor::stop(void) {
  DataFileReader::stop();
  delete thReader;
}

bool FileReaderProcessor::isActive(void) { return thReader->isActive(); }

}
