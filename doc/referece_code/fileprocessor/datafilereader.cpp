/**
 * Project:
 * Subproject: tracrecons
 * File name:  datafilereader.cpp
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
#include "datafilereader.h"
#include "logmacros.h"
#include <unistd.h>
#include <cstdlib>
#include <fstream>

// Ver "main.cpp"
extern bool orbitDataInserted;

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
DataFileReader::DataFileReader(I_Pusher &dataOut, GlobalDataPath &gdp,
                               string name, char tkSep) :
  I_Task(), dataOut(dataOut), gdp(gdp)
{
  setFileName(name);
  setTokenSeparator(tkSep);
  reset();
}

DataFileReader::~DataFileReader() { }

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void DataFileReader::run(void) throw (CException) {
  LOGTRACEFN_IN;

  ifstream inputFile;

  inputFile.open(fileName.c_str());
  if ( !inputFile.is_open() )
    throw (CException("No localizo el fichero"));

  string currentLine;
  int superLayer, layer, channel, tdcTStamp, orbit;
  double payloads[PAYLOAD_ENTRIES];
  DTPrimitive *dtPrim = NULL;
  active = true;

  int lineCounter = 0;
  while ( (!inputFile.eof()) && active ){
    stats.totalLines++;
    getline(inputFile, currentLine);

    if( !(currentLine == "" || currentLine == "\r") ) {
      try {
        parseLine(currentLine, &superLayer, &layer, &channel, &tdcTStamp, 
                  &orbit, payloads);

        dtPrim = new DTPrimitive();
        dtPrim->setSuperLayerId(superLayer);
        dtPrim->setLayerId(layer);
        dtPrim->setChannelId(channel);
        dtPrim->setTDCTime(tdcTStamp);
        dtPrim->setOrbit(orbit);
        for (int i = 0; i < PAYLOAD_ENTRIES; i++)
          dtPrim->setPayload(payloads[i], i);

        dataOut.push(dtPrim);
      }
      catch (CException e) {
        LOGFATAL(NString("Error en linea de fichero: ").sAppend(currentLine));
      }
    }
    else {
      
      // Debe ser reseteado por el MIXER.
      orbitDataInserted = true;
      
      if (lineCounter % 500 == 0) cerr << "Segmento: " << lineCounter << endl;
      lineCounter++;
      /*
       * This is an alternative mechanism to wait between input segments,
       * to avoid filling up internal buffers, based on looking if those
       * buffers are empty or not. Besides it also checks if the certain
       * time-out has been surpassed.
       */
//       int timeCounter = 0;
      while (true) {
        usleep(9000);
//         timeCounter++;
//         if (timeCounter >= 17500 || !gdp.isThereDataInAnyChannel()) break;
//         if ( !gdp.isThereDataInAnyMiddleChannel() && 
//               gdp.areEmptyPreviousBuffers() )
//           break;

        if ( !orbitDataInserted ) break;
        
      }
    }
  }

  inputFile.close();
  endFile = true;

  LOGTRACEFN_OUT;
}

void DataFileReader::stop(void) { active = false; }

void DataFileReader::reset(void) {
  stats.totalLines = 0;
  endFile = false;
}

bool DataFileReader::isEndOfFile(void) {
   return endFile;
}

/** Recorta espacios por la derecha
 */
string DataFileReader::rightTrim (string value){

  while(value.at(value.length() - 1) == ' ')
    value.replace(value.length() - 1, 1, "");

  return value;
}

/** Recorta espacios por la izquierda
 */
string DataFileReader::leftTrim (string value){

  while(value.at(0) == ' ') value.replace(0, 1, "");

  return value;
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void DataFileReader::setFileName(string name) { fileName = name; }

void DataFileReader::setTokenSeparator(char separator) {
  tokenSeparator = separator;
}

const DataFileReader::STATISTICS* DataFileReader::getStatistics(void) {
  return &stats;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void DataFileReader::parseLine(string line,  int *superLayer, int *layer, 
                               int *channel, int *tdcTStamp,  int *orbit, 
                               double *payloads)
  throw (CException)
{
  int sepPos;
  string myLine, auxStr;

  /* Busqueda de los 3 datos en cada linea */
  myLine   = line;
  sepPos   = myLine.find(tokenSeparator);
  auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

  *superLayer   = atoi(auxStr.c_str());
  if(*superLayer == 0 && thereIsANotNullNumber(auxStr))
    throw (CException(NString("Datos no validos: ").sAppend(line)));

  myLine   = myLine.substr(sepPos + 1);
  sepPos   = myLine.find(tokenSeparator);
  auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

  *layer   = atoi(auxStr.c_str());
  if(*layer == 0 && thereIsANotNullNumber(auxStr))
    throw (CException(NString("Datos no validos: ").sAppend(line)));

  myLine     = myLine.substr(sepPos + 1);
  sepPos     = myLine.find(tokenSeparator);
  auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

  *channel = atoi(auxStr.c_str());
  if(*channel == 0 && thereIsANotNullNumber(auxStr))
    throw (CException(NString("Datos no validos: ").sAppend(line)));

  myLine  = myLine.substr(sepPos + 1);
  sepPos  = myLine.find(tokenSeparator);
  auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

  *tdcTStamp = atoi(auxStr.c_str());
  if(*tdcTStamp == 0 && thereIsANotNullNumber(auxStr))
    throw (CException(NString("Datos no validos: ").sAppend(line)));
  
  myLine  = myLine.substr(sepPos + 1);
  sepPos  = myLine.find(tokenSeparator);
  auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

  *orbit = atoi(auxStr.c_str());
  if(*orbit == 0 && thereIsANotNullNumber(auxStr))
    throw (CException(NString("Datos no validos: ").sAppend(line)));
  
  /* Lectura de los "payloads", hasta el máximo de PAYLOAD_ENTRIES */
  for (int i=0; i < PAYLOAD_ENTRIES; i++) {
    myLine  = myLine.substr(sepPos + 1);
    sepPos  = myLine.find(tokenSeparator);
    auxStr = leftTrim(rightTrim(myLine.substr(0, sepPos)));

    payloads[i] = atof(auxStr.c_str());
    if(payloads[i] == 0 && thereIsANotNullNumber(auxStr))
      throw (CException(NString("Datos no validos: ").sAppend(line)));

    if (sepPos == -1) break;              // No more tokens, or token not found
  }

}

bool DataFileReader::thereIsANotNullNumber(string str) {

  string::const_iterator it = str.begin();

  while (it != str.end()) {
    if (isdigit(*it) && *it != '0') return true;
    else it++;
  }
  return false;
}


}
