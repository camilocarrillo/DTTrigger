/**
 * Project:
 * Subproject: tracrecons
 * File name:  mpqualityfilter.cpp
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
#include "mpqualityfilter.h"
#include "logmacros.h"
#include "muonpath.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
MPQualityFilter::MPQualityFilter(I_Popper &dataIn, I_Pusher &dataOut) :
  I_Filter(dataIn, dataOut)
{
  LOGTRACE("Creando un 'MPQualityFilter'");
}

MPQualityFilter::~MPQualityFilter()
{
  LOGTRACE("Destruyendo un 'MPQualityFilter'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void MPQualityFilter::filter(void) {

  MuonPath *mPath = (MuonPath*) dataIn.pop();
  /*
    Esta línea se incluye para evitar que, tras un 'stop', que fuerza la
    liberación del mutex de la fifo de entrada, devuelva un puntero nulo.
   */
  if (mPath == NULL) return;
  if (mPath->getQuality() >= quality) dataOut.push(mPath);
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void MPQualityFilter::setMinQuality(MP_QUALITY q) { quality = q; }
MP_QUALITY MPQualityFilter::getMinQuality(void) { return quality; }

}
