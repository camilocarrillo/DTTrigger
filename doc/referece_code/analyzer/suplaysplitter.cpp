/**
 * Project:
 * Subproject: tracrecons
 * File name:  suplaysplitter.cpp
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
 * Copyright (c) 2015-04-14 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "suplaysplitter.h"
#include "muonpath.h"
#include "logmacros.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
SupLayerSplitter::SupLayerSplitter(I_Popper &dataIn)
  : I_Task(), dataIn(dataIn)
{
  LOGTRACE("Creando un 'SupLayerSplitter'");
  setRRobin(false);
}

SupLayerSplitter::~SupLayerSplitter() {
  LOGTRACE("Destruyendo un 'SupLayerSplitter'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void SupLayerSplitter::run(void)
{
  LOGTRACEFN_IN;

  while(roundRobin)
    splitData();

  LOGTRACEFN_OUT;
}

void SupLayerSplitter::stop(void) {
  setRRobin(false);
  dataIn.reset();
  resetOutChannels();
}

void SupLayerSplitter::setRRobin(bool rr) { roundRobin = rr; }

void SupLayerSplitter::splitData(void) {

  MuonPath* ptrMPath = (MuonPath*) dataIn.pop();

  /* To avoid NULL pointer after a 'stop' command */
  if (ptrMPath == NULL) return;

  int superLay = 0;
  if (ptrMPath->getPrimitive(0)->isValidTime())
    superLay = ptrMPath->getPrimitive(0)->getSuperLayerId();
  else
    superLay = ptrMPath->getPrimitive(1)->getSuperLayerId();

  (qslFilterOut[superLay])->push(ptrMPath);
}

void SupLayerSplitter::setOutChannels(QSLFilterPath *qslFilterOut[NUM_SUPERLAYERS]) 
{
  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++)
    this->qslFilterOut[supLay] = qslFilterOut[supLay];
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void SupLayerSplitter::resetOutChannels(void) {
  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++)
      (this->qslFilterOut[supLay])->reset();
}

}
