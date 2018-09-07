/**
 * Project:
 * Subproject: tracrecons
 * File name:  filewriterprocessor.cpp
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
#include "filewriterprocessor.h"

namespace CMS {

FileWriterProcessor::FileWriterProcessor(I_Popper &dataIn, string name,
                                         string tkSep)
  : DataFileWriter(dataIn, name, tkSep)
{

}

FileWriterProcessor::~FileWriterProcessor() { }

void FileWriterProcessor::start(void) {
  thWriter = new Thread(10);
  this->setRRobin(true);
  thWriter->setCtrlJobExceptions(false);
  thWriter->doThisJob(*this);
}

void FileWriterProcessor::stop(void) {
  DataFileWriter::stop();
  delete thWriter;
}

bool FileWriterProcessor::isActive(void) { return thWriter->isActive(); }


}
