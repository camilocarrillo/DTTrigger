/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  globaldatapath.cpp
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
#include "globaldatapath.h"
#include "logmacros.h"
#include <unistd.h>

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
GlobalDataPath::GlobalDataPath(unsigned int maxMidFifoSize) :
  I_Pusher(), I_Popper(),
  dtpInFifo(maxMidFifoSize),
  dtpTimeFilteredFifo(maxMidFifoSize),
  twDiscriminator(dtpInFifo, dtpTimeFilteredFifo),
  chSplitter(dtpTimeFilteredFifo, splittedChannels),
  candidateMuonPathFifo(maxMidFifoSize),
  dataMixer(splittedChannels, candidateMuonPathFifo, twDiscriminator),
  validMuonPathFifo(maxMidFifoSize),
  dataAnalyzer(candidateMuonPathFifo, validMuonPathFifo),
  filteredValidMPFifo(maxMidFifoSize),
  dupMPValidFilter(validMuonPathFifo, filteredValidMPFifo, 64),
  postAnPath(filteredValidMPFifo, maxMidFifoSize),
  thTimeWDisc(),
  thChSplit(),
  thDataMix(),
  thDataAnal(),
  thValMPFilter()
{
  LOGTRACE("Creando un 'GlobalDataPath'");
  twDiscriminator.setRejectTimeWindow(0);
  twDiscriminator.setTimeWindow(2500);

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c] = new SeekTimeBuffer( (l+1)*SIZE_SEEKT_BUFFER, l, c );

  chSplitter.setOutChannels(splittedChannels);
  dataMixer.setInChannels(splittedChannels);
}

GlobalDataPath::~GlobalDataPath() {
  LOGTRACE("Destruyendo un 'GlobalDataPath'");

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      delete splittedChannels[l][c];
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void GlobalDataPath::push(I_Object* obj) {
  LOGTRACEFN_IN;

  dtpInFifo.push(obj);
}

I_Object* GlobalDataPath::pop(void) {
  LOGTRACEFN_IN;

  return postAnPath.pop();
}

void GlobalDataPath::discriminate(void) { twDiscriminator.filter();  }
void GlobalDataPath::splitData(void)    { chSplitter.splitData();    }
void GlobalDataPath::mixData(void)      { dataMixer.mixData();       }
void GlobalDataPath::analyze(void)      { dataAnalyzer.analyze();    }

void GlobalDataPath::start(void) {
  thDataAnal    = new Thread(1);
  thDataMix     = new Thread(2);
  thChSplit     = new Thread(3);
  thTimeWDisc   = new Thread(4);
  thValMPFilter = new Thread(5);

  dataAnalyzer.setRRobin(true);
  dataMixer.setRRobin(true);
  chSplitter.setRRobin(true);
  twDiscriminator.setRRobin(true);
  dupMPValidFilter.setRRobin(true);

  thDataAnal->doThisJob(dataAnalyzer);
  thDataMix->doThisJob(dataMixer);
  thChSplit->doThisJob(chSplitter);
  thTimeWDisc->doThisJob(twDiscriminator);
  thValMPFilter->doThisJob(dupMPValidFilter);
  
  postAnPath.start();
}

void GlobalDataPath::stop(void) {
  twDiscriminator.stop();
  chSplitter.stop();
  dataMixer.stop();
  dataAnalyzer.stop();
  dupMPValidFilter.stop();

  postAnPath.stop();
  usleep(500000);

  reset();
  delete thDataAnal;
  reset();
  delete thDataMix;
  reset();
  delete thChSplit;
  reset();
  delete thTimeWDisc;
  reset();
  delete thValMPFilter;
}

void GlobalDataPath::reset(void) {
  dtpInFifo.reset();
  twDiscriminator.reset();
  dtpTimeFilteredFifo.reset();

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c]->reset();

  dataMixer.reset();
  candidateMuonPathFifo.reset();
  dataAnalyzer.reset();
  validMuonPathFifo.reset();
  dupMPValidFilter.reset();
  filteredValidMPFifo.reset();
  postAnPath.reset();
  chSplitter.resetStats();
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void GlobalDataPath::setMixerForcedTimeIncrement(int time) {
  dataMixer.setForcedTimeIncrement(time);
}

void GlobalDataPath::setDiscriminationTimeWindow(int tWindow) {
  twDiscriminator.setRejectTimeWindow(tWindow);
}

int GlobalDataPath::getDiscriminationTimeWindow(void) {
  return twDiscriminator.getRejectTimeWindow();
}

void GlobalDataPath::setAcceptanceTimeWindow(int tWindow) {
  twDiscriminator.setTimeWindow(tWindow);
}

int GlobalDataPath::getAcceptanceTimeWindow(void) {
  return twDiscriminator.getTimeWindow();
}

void GlobalDataPath::setTimeBufferTWindow(int tWindow) {
  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c]->setTimeWindow(tWindow);

  dataMixer.setSeekTimeBufWindowRelBeginning(tWindow);
}

int GlobalDataPath::getTimeBufferTWindow(void) {
  return splittedChannels[0][0]->getTimeWindow();
}

void GlobalDataPath::setPathTolerance(int tolerance) {
//   dataAnalyzer.setPathTolerance(tolerance);
}

int GlobalDataPath::getPathTolerance(void) {
//   return dataAnalyzer.getPathTolerance();
  return 0;
}

void GlobalDataPath::setBXTolerance(int tolerance) {
  dataAnalyzer.setBXTolerance(tolerance);
}

int GlobalDataPath::getBXTolerance(void) {
  return dataAnalyzer.getBXTolerance();
}

void GlobalDataPath::setMinMuonPQuality(MP_QUALITY q) {
  dataAnalyzer.setMinimumQuality(q);
}

MP_QUALITY GlobalDataPath::getMinMuonPQuality(void) {
  return dataAnalyzer.getMinimumQuality();
}

void GlobalDataPath::setChiSquareThreshold(float ch2Thr) {
  dataAnalyzer.setChiSquareThreshold(ch2Thr);
}

const GlobalDataPath::STATISTICS* GlobalDataPath::getStatistics(void) {
//  stats.twdStats = *twDiscriminator.getStatistics();
//  stats.cspStats = *chSplitter.getStatistics();

  return &stats;
}

unsigned int GlobalDataPath::getNumCandidateMuonPaths(void) {
  return candidateMuonPathFifo.size();
}

unsigned int GlobalDataPath::getNumValidMuonPaths(void) {
  return validMuonPathFifo.size();
}

bool GlobalDataPath::isThereDataInAnyMiddleChannel(void) {
  return dataMixer.isThereDataInAnyChannel();
}

bool GlobalDataPath::areEmptyPreviousBuffers(void) {
  return (
    dtpInFifo.empty()                &&
    dtpTimeFilteredFifo.empty()
  );
}

bool GlobalDataPath::areEmptySegmentBuffers(void) {
  return (
    candidateMuonPathFifo.empty()    &&
    validMuonPathFifo.empty()        &&
    filteredValidMPFifo.empty()      &&
    postAnPath.empty()
  );
}

}

