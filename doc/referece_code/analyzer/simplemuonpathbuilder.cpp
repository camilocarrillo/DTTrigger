/**
 * Project:
 * Subproject: tracrecons
 * File name:  simplemuonpathbuilder.cpp
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
 * Copyright (c) 2015-09-24 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "simplemuonpathbuilder.h"
#include "logmacros.h"
#include "muonpath.h"

namespace CMS {

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
SimpleMuonPathBuilder::SimpleMuonPathBuilder(I_Popper &inDTPrim,
                                             I_Pusher &outMuonPath) :
  inDTPrim(inDTPrim), outMuonPath(outMuonPath)
{
  setRRobin(false);
}

SimpleMuonPathBuilder::~SimpleMuonPathBuilder() { }

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void SimpleMuonPathBuilder::run(void) {
  LOGTRACEFN_IN;

  while(roundRobin)
    buildData();

  LOGTRACEFN_OUT;
}

void SimpleMuonPathBuilder::stop(void) { setRRobin(false); }
void SimpleMuonPathBuilder::setRRobin(bool rr) { roundRobin = rr; }

/**
 * Builds a MuonPath using 4 DTPrimitives, assuming that each 4 of them, got
 * from input FIFO, form a MuonPath, i.e, they came from right layers and
 * right channels, compatible with one of the possible standard paths within
 * a BTI
 */
void SimpleMuonPathBuilder::buildData(void) {

  DTPrimitive *ptrPrimitive[4];

  for (int i = 0; i <= 3; i++){
    ptrPrimitive[i] = (DTPrimitive*) inDTPrim.pop();

    /* Comprobación para evitar que, cuando se para el programa, se aborte
       por un puntero nulo */
    if (ptrPrimitive[i] == NULL) return;

    if(ptrPrimitive[i]->getLayerId() != i) {
      LOGERROR("'Layer' de primitiva desalineada en fichero de datos");
      return;
    }
  }

  int layout[4];

  discoverHorizontalLayout(ptrPrimitive, layout);

  for (int i = 0; i <= 3; i++){
    if (layout[i] > 3 || layout[i] < -3) {
      LOGERROR("'Layout' de primitivas demasiado grande (> 3)");
      return;
    }
  }

  MuonPath *ptrMuonPath = new MuonPath(ptrPrimitive);
  ptrMuonPath->setCellHorizontalLayout(layout);

  outMuonPath.push( ptrMuonPath );
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void SimpleMuonPathBuilder::discoverHorizontalLayout(DTPrimitive* DTprim[4],
                                                     int *layout)
{
  int base = DTprim[0]->getChannelId();

  layout[0] = 0;
  layout[1] = 2*(DTprim[1]->getChannelId() - base) - 1;
  layout[2] = 2*(DTprim[2]->getChannelId() - base);
  layout[3] = 2*(DTprim[3]->getChannelId() - base) - 1;
}

}
