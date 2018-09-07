/**
 * Project:
 * Subproject: tracrecons
 * File name:  segmentmatcher.cpp
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
#include "segmentmatcher.h"
#include "muonpath.h"
#include "logmacros.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
SegmentMatcher::SegmentMatcher(I_Pusher &dataOut)
  : I_Task(), dataOut(dataOut)
{
  LOGTRACE("Creando un 'SegmentMatcher'");
  setRRobin(false);
}

SegmentMatcher::~SegmentMatcher() {
  LOGTRACE("Destruyendo un 'SegmentMatcher'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void SegmentMatcher::run(void)
{
  LOGTRACEFN_IN;

  while(roundRobin)
    matchSegments();

  LOGTRACEFN_OUT;
}

void SegmentMatcher::stop(void) {
  setRRobin(false);
  resetInChannels();
  dataOut.reset();
}

void SegmentMatcher::setRRobin(bool rr) { roundRobin = rr; }

/*
 * IMPORTANTE !!!!!!
 *
 * DE MOMENTO EL CÓDIGO NO HACE MATCHING DE SEGMENTOS. Solo fusiona los
 * resultados de las FIFO de entrada en la de salida haciendo round-robin
 * sobre ellas.
 *
 * Esta versión es para probar que los filtros de las 3 superlayer por
 * separado funcionan correctamente, y el resto del código y los threads de
 * proceso hacen los que deben.
 *
 */
void SegmentMatcher::matchSegments(void) {

  // Round-robin por las 3 SuperLayers
  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++) {
    
    // Ponemos un máximo para que no se eternice leyendo de una SL.
    int maxSegCount = 20;

    while( ! (qslFilterIn[supLay])->empty() && maxSegCount > 0 ) {

      MuonPath *ptrMPath = (MuonPath*) qslFilterIn[supLay]->pop();
      dataOut.push(ptrMPath);

      maxSegCount--;
    }
  }
}

/*
 * CHAPUZA !!!! Hay que llamar a este método para fijar los punteros a los
 * "path" de entrada ANTES DE PONERLO A FUNCIONAR ("run")
 */
void SegmentMatcher::setInChannels(QSLFilterPath *qslFilterIn[3]) {
  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++)
    this->qslFilterIn[supLay] = qslFilterIn[supLay];
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void SegmentMatcher::resetInChannels(void) {
  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++)
      (this->qslFilterIn[supLay])->reset();
}

}
