/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  postanalisispath.cpp
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
 * Copyright (c) 2015-02-11 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "postanalisispath.h"
#include "logmacros.h"
#include <unistd.h>

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
PostAnalisisPath::PostAnalisisPath(I_Popper& dataIn,
                                   unsigned int maxMidFifoSize) :
  I_Popper(),
  qslPathLayerPHIL(maxMidFifoSize),
  qslPathLayerTHETA(maxMidFifoSize),
  qslPathLayerPHIH(maxMidFifoSize),
  slSplitter(dataIn),
  outSegmentFifo(maxMidFifoSize),
  sgMatch(outSegmentFifo),
  thSLSplitter(),
  thSegmentMatch()
{
  LOGTRACE("Creando un 'PostAnalisisPath'");

  // Conectando objetos con punteros.
  qslLayers[0] = &qslPathLayerPHIL;
  qslLayers[1] = &qslPathLayerTHETA;
  qslLayers[2] = &qslPathLayerPHIH;

  slSplitter.setOutChannels(qslLayers);
  sgMatch.setInChannels(qslLayers);
}

PostAnalisisPath::~PostAnalisisPath() {
  LOGTRACE("Destruyendo un 'PostAnalisisPath'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
I_Object* PostAnalisisPath::pop(void) {
  LOGTRACEFN_IN;

  return outSegmentFifo.pop();
}

void PostAnalisisPath::start(void) {
  thSLSplitter   = new Thread();
  thSegmentMatch = new Thread();

  slSplitter.setRRobin(true);
  sgMatch.setRRobin(true);

  qslPathLayerPHIL.start();
  qslPathLayerTHETA.start();
  qslPathLayerPHIH.start();

  thSLSplitter->doThisJob(slSplitter);
  thSegmentMatch->doThisJob(sgMatch);
}

void PostAnalisisPath::stop(void) {
  slSplitter.stop();
  qslPathLayerPHIL.stop();
  qslPathLayerTHETA.stop();
  qslPathLayerPHIH.stop();
  sgMatch.stop();

  reset();
  delete thSLSplitter;
  reset();
  delete thSegmentMatch;
}

void PostAnalisisPath::reset(void) {
  qslPathLayerPHIL.reset();
  qslPathLayerTHETA.reset();
  qslPathLayerPHIH.reset();
  outSegmentFifo.reset();
}

bool PostAnalisisPath::empty(void) {
  return (
    qslPathLayerPHIL.empty()  &&
    qslPathLayerTHETA.empty() &&
    qslPathLayerPHIH.empty()  &&
    outSegmentFifo.empty()
  );
}

}

