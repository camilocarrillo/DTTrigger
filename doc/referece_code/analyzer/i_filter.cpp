/**
 * Project:
 * Subproject: tracrecons
 * File name:  i_filter.cpp
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
 * Copyright (c) 2015-07-02 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "i_filter.h"
#include "logmacros.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
I_Filter::I_Filter(I_Popper &dataIn, I_Pusher &dataOut) :
  I_Task(), dataIn(dataIn), dataOut(dataOut)
{
  LOGTRACE("Creando un 'I_Filter'");
  setRRobin(false);
}

I_Filter::~I_Filter() {
  LOGTRACE("Destruyendo un 'I_Filter'");
}
//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void I_Filter::run(void) {
  LOGTRACEFN_IN;

  while(roundRobin)
    filter();

  LOGTRACEFN_OUT;
}

void I_Filter::stop(void) {
  setRRobin(false);
  dataIn.reset();
}

void I_Filter::filter(void) { }

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void I_Filter::setRRobin(bool rr) { roundRobin = rr; }

}
