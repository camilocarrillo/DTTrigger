/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  qslfilterpath.cpp
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
#include "qslfilterpath.h"
#include "logmacros.h"
#include <unistd.h>

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
QSLFilterPath::QSLFilterPath(unsigned int maxMidFifoSize) :
  I_Pusher(), I_Popper(),
  validSegmentFifo(maxMidFifoSize),
  eQualityMPFilterST1(validSegmentFifo, enhancedQualityMPFifoST1),
  enhancedQualityMPFifoST2(maxMidFifoSize),
  eQualityMPFilterST2(enhancedQualityMPFifoST1, enhancedQualityMPFifoST2),
  thEnQMPFilterST1(),
  thEnQMPFilterST2()
{
  LOGTRACE("Creando un 'QSLFilterPath'");
}

QSLFilterPath::~QSLFilterPath() {
  LOGTRACE("Destruyendo un 'QSLFilterPath'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void QSLFilterPath::push(I_Object* obj) {
  LOGTRACEFN_IN;

  validSegmentFifo.push(obj);
}

I_Object* QSLFilterPath::pop(void) {
  LOGTRACEFN_IN;

  return enhancedQualityMPFifoST2.pop();
}

void QSLFilterPath::start(void) {
  thEnQMPFilterST1 = new Thread();
  thEnQMPFilterST2 = new Thread();

  eQualityMPFilterST1.setRRobin(true);
  eQualityMPFilterST2.setRRobin(true);

  thEnQMPFilterST1->doThisJob(eQualityMPFilterST1);
  thEnQMPFilterST2->doThisJob(eQualityMPFilterST2);
}

void QSLFilterPath::stop(void) {
  eQualityMPFilterST1.stop();
  /* 
   * Artificio para dar tiempo a que el último elemento del último 
   * de la cadena de proceso envíe el último dato que tenga encolado y que no 
   * se acabará de procesar debido a la parada del programa.
   * 0.5 seg debería ser suficiente para que todo se pare ordenadamente.
   */
  usleep(500000);

  eQualityMPFilterST2.stop();
  usleep(500000);
  
  reset();
  delete thEnQMPFilterST1;
  reset();
  delete thEnQMPFilterST2;
}

void QSLFilterPath::reset(void) {
  validSegmentFifo.reset();
  enhancedQualityMPFifoST1.reset();
  enhancedQualityMPFifoST2.reset();
}

bool QSLFilterPath::empty(void) {
  return (
    validSegmentFifo.empty()         &&
    enhancedQualityMPFifoST1.empty() &&
    enhancedQualityMPFifoST2.empty()
  );
}

}

