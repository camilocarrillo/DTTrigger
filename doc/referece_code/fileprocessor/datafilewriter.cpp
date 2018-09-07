/**
 * Project:
 * Subproject: tracrecons
 * File name:  datafilewriter.cpp
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
#include "datafilewriter.h"
#include "logmacros.h"
#include "constants.h"
#include <fstream>
#include <cstdio>
#include <math.h>
#include <iomanip>

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
DataFileWriter::DataFileWriter(I_Popper &inMuonPath, string name, string tkSep)
  : I_Task(), inMuonPath(inMuonPath)
{
  lineCounter = 0;
  lineBuffer  = "";

  setFileName(name);
  setRRobin(false);
  setTokenSeparator(tkSep);
  setNumLinesBufferSize();
}

DataFileWriter::~DataFileWriter() { }

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void DataFileWriter::run(void) {
  LOGTRACEFN_IN;

  ofstream outputFile;
  outputFile.open(fileName.c_str(), ios::out|ios::trunc);

#ifndef PRUEBA_MEZCLADOR
  outputFile << "                       ANALYZED DATA.\n"
             << "===========================================================\n"
             << "Qlty."         << tokenSeparator
             << "BXTime"        << tokenSeparator
             << "#BX"           << tokenSeparator
             << "Sup.Lay."      << tokenSeparator
             << "Chan[0]."      << tokenSeparator
             << "Chan[1]."      << tokenSeparator
             << "Chan[2]."      << tokenSeparator
             << "Chan[3]."      << tokenSeparator
             << "TDC[0]"        << tokenSeparator
             << "TDC[1]"        << tokenSeparator
             << "TDC[2]"        << tokenSeparator
             << "TDC[3]"        << tokenSeparator
             << "Lat.Cmb[0]"    << tokenSeparator
             << "Lat.Cmb[1]"    << tokenSeparator
             << "Lat.Cmb[2]"    << tokenSeparator
             << "Lat.Cmb[3]"    << tokenSeparator
             << "X(mm)"         << tokenSeparator
             << "tan(phi)"      << tokenSeparator
             << "chi-square"    << tokenSeparator;


  for(int j = 0; j < PAYLOAD_ENTRIES; j++) {
    for(int i = 0; i <= 3; i++) {
      if (j != 0 && i != 0) break;
      
      outputFile << "HTag[" << i << "," << j << "]";
      outputFile << tokenSeparator;
    }
  }

  outputFile << "\n";
#endif

  if ( !outputFile.is_open() )
    throw (CException("No es posible crear el fichero de salida"));

  while(roundRobin)
    procesar(outputFile);

  if (lineBuffer != "" || lineCounter != 0) outputFile << lineBuffer;

  outputFile.flush();
  outputFile.close();

  /* After stopping it's necessary to reset internal buffers to avoid
  data overlapping a duplications during stop/start program cycles */
  lineCounter = 0;
  lineBuffer  = "";

  LOGTRACEFN_OUT;
}

void DataFileWriter::procesar(ofstream &file) {

  MuonPath *mPath = (MuonPath*) inMuonPath.pop();
  if (mPath == NULL) return;

#ifdef PRUEBA_MEZCLADOR
  lineBuffer += buildTextLineForTest(mPath);
#else
  lineBuffer += buildTextLine(mPath);
#endif

  lineBuffer += "\n";
  lineCounter++;

  if (lineCounter >= writeAfterNumLines) {
    file << lineBuffer;
    file.flush();

    lineCounter = 0;
    lineBuffer  = "";
  }
  delete mPath;
}

void DataFileWriter::stop(void) {
  setRRobin(false);
  inMuonPath.reset();
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void DataFileWriter::setFileName(string name) { fileName = name; }
void DataFileWriter::setRRobin(bool rr) { roundRobin = rr; }

void DataFileWriter::setTokenSeparator(string separator) {
  tokenSeparator = separator;
}

void DataFileWriter::setNumLinesBufferSize(int numLines) {
  if (numLines >= 1) writeAfterNumLines = numLines;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
string DataFileWriter::buildTextLine(MuonPath* const mPath) {

  string line = "";
  char buffer[40];
  double payload;
  
  sprintf(buffer, "%d", mPath->getQuality());
  line += buffer;
  line = addSeparator(line);

  sprintf(buffer, "%d", mPath->getBxTimeValue());
  line += buffer;
  line = addSeparator(line);

  sprintf(buffer, "%4d", mPath->getBxNumId());
  line += buffer;
  line = addSeparator(line);
  /*
   * Si el primer HIT no es válido, el segundo ha de serlo porque 
   * 3 HITS tiene que haber obligatoriamente válido en un segmento.
   */
  if (mPath->getPrimitive(0)->isValidTime())
    sprintf(buffer, "%4d", mPath->getPrimitive(0)->getSuperLayerId());
  else 
    sprintf(buffer, "%4d", mPath->getPrimitive(1)->getSuperLayerId());
  
  line += buffer;
  line = addSeparator(line);

  /*
  * Modificación para que, en vez del valor "-1" de los casos "dummy", 
  * aparezca un "999" que le resulta más fácil de manejar a J. Troconiz.
  */
  int value;
  for(int i = 0; i <= 3; i++) {
    value = mPath->getPrimitive(i)->getChannelId();
    value = (value >= 0) ? value : 999;
    sprintf(buffer, "%0.2d", value);
    line += buffer;
    line = addSeparator(line);
  }

  for(int i = 0; i <= 3; i++) {
    sprintf(buffer, "%d", mPath->getPrimitive(i)->getTDCTime());
    line += buffer;
    line = addSeparator(line);
  }

  const LATERAL_CASES* latComb = mPath->getLateralComb();
  for(int i = 0; i <= 3; i++) {
    sprintf(buffer, "%1d", latComb[i]);
    line += buffer;
    line = addSeparator(line);
  }

  sprintf(buffer, "%.3f", mPath->getHorizPos());
  line += buffer;
  line = addSeparator(line);

  sprintf(buffer, "%.5f", mPath->getTanPhi());
  line += buffer;
  line = addSeparator(line);

  sprintf(buffer, "%.5f", mPath->getChiSq());
  line += buffer;
  line = addSeparator(line);

  /* Additional data for tracing and analyzing */
  for(int j = 0; j < PAYLOAD_ENTRIES; j++) {
    int sel = (mPath->getPrimitive(0)->isValidTime()) ? 0 : 1;

    for(int i = 0; i <= 3; i++) {
      if (j != 0 && i != 0) break;
      
      if (j == 0) payload = mPath->getPrimitive(i)->getPayload(j);
      else        payload = mPath->getPrimitive(sel)->getPayload(j);

      double diff = fabsf(round(payload) - payload);
      if (diff <= 0.0001f) {
        if (payload == 0.0) sprintf(buffer, "%i", 0);
        else                sprintf(buffer, "%.0f", payload);
      }
      else sprintf(buffer, "%.5g", payload);

      line += buffer;
      line  = addSeparator(line);
    }
  }

  return line;
}

/**
 * Este método construye una línea con los datos del 'MuonPath' con solo los
 * valores del 'timestamp' para hacer análisis de las mezclas de datos que
 * realiza el Mixer.
 * En cada línea se muestran los 4 timstamps, en orden de capa: a la izquierda
 * la capa 0.
 */
string DataFileWriter::buildTextLineForTest(MuonPath* const mPath) {

  string line = "";
  char buffer[40];

  for(int i = 0; i <= 3; i++) {
    sprintf(buffer, "%d", mPath->getPrimitive(i)->getTDCTime());
    line += buffer;
    if (i < 3) line = addSeparator(line);
  }

  return line;
}

string DataFileWriter::addSeparator(string line) {
  string lnAux = line;

  lnAux += tokenSeparator;

  return lnAux;
}

}
