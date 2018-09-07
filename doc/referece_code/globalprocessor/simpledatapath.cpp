/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  simpledatapath.cpp
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
#include "simpledatapath.h"
#include "logmacros.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
SimpleDataPath::SimpleDataPath(unsigned int maxMidFifoSize) :
  I_Pusher(), I_Popper(),
  dtpInFifo(maxMidFifoSize),
  candidateMuonPathFifo(maxMidFifoSize),
  mpBuilder(dtpInFifo, candidateMuonPathFifo),
  validMuonPathFifo(maxMidFifoSize),
  dataAnalyzer(candidateMuonPathFifo, validMuonPathFifo),
  thMPBuild(),
  thDataAnal()
{
  LOGTRACE("Creando un 'SimpleDataPath'");
}

SimpleDataPath::~SimpleDataPath() {
  LOGTRACE("Destruyendo un 'SimpleDataPath'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void SimpleDataPath::push(I_Object* obj) {
  LOGTRACEFN_IN;

  dtpInFifo.push(obj);
}

I_Object* SimpleDataPath::pop(void) {
  LOGTRACEFN_IN;

  return validMuonPathFifo.pop();
}

void SimpleDataPath::buildData(void) { mpBuilder.buildData(); }
void SimpleDataPath::analyze(void) { dataAnalyzer.analyze(); }

void SimpleDataPath::start(void) {
  thDataAnal  = new Thread(1);
  thMPBuild   = new Thread(2);

  dataAnalyzer.setRRobin(true);
  mpBuilder.setRRobin(true);

  thDataAnal->doThisJob(dataAnalyzer);
  thMPBuild->doThisJob(mpBuilder);
}

void SimpleDataPath::stop(void) {
  mpBuilder.stop();
  dataAnalyzer.stop();

  reset();
  delete thDataAnal;
  reset();
  delete thMPBuild;
}

void SimpleDataPath::reset(void) {
  dtpInFifo.reset();
  candidateMuonPathFifo.reset();
  dataAnalyzer.reset();
  validMuonPathFifo.reset();
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void SimpleDataPath::setPathTolerance(int tolerance) {
//   dataAnalyzer.setPathTolerance(tolerance);
}

int SimpleDataPath::getPathTolerance(void) {
//   return dataAnalyzer.getPathTolerance();
  return 0;
}

void SimpleDataPath::setBXTolerance(int tolerance) {
  dataAnalyzer.setBXTolerance(tolerance);
}

int SimpleDataPath::getBXTolerance(void) {
  return dataAnalyzer.getBXTolerance();
}

void SimpleDataPath::setMinMuonPQuality(MP_QUALITY q) {
  dataAnalyzer.setMinimumQuality(q);
}

MP_QUALITY SimpleDataPath::getMinMuonPQuality(void) {
  return dataAnalyzer.getMinimumQuality();
}

}

