/**
 * Project:
 * Subproject: tracrecons
 * File name:  noanalyzerdatapath.cpp
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
 * Copyright (c) 2016-07-14 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "noanalyzerdatapath.h"
#include "logmacros.h"

namespace CMS {

NoAnalyzerDataPath::NoAnalyzerDataPath(unsigned int maxMidFifoSize) :
  I_Pusher(), I_Popper(),
  dtpInFifo(maxMidFifoSize),
  dtpTimeFilteredFifo(maxMidFifoSize),
  twDiscriminator(dtpInFifo, dtpTimeFilteredFifo),
  chSplitter(dtpTimeFilteredFifo, splittedChannels),
  candidateMuonPathFifo(maxMidFifoSize),
  dataMixer(splittedChannels, candidateMuonPathFifo,twDiscriminator),
  thTimeWDisc(),
  thChSplit(),
  thDataMix()
{
  LOGTRACE("Creando un 'NoAnalyzerDataPath'");

  twDiscriminator.setRejectTimeWindow(0);
  twDiscriminator.setTimeWindow(2500);

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c] = new SeekTimeBuffer(SIZE_SEEKT_BUFFER, l, c);

  chSplitter.setOutChannels(splittedChannels);
  dataMixer.setInChannels(splittedChannels);
}

NoAnalyzerDataPath::~NoAnalyzerDataPath() {
  LOGTRACE("Destruyendo un 'NoAnalyzerDataPath'");

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      delete splittedChannels[l][c];
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void NoAnalyzerDataPath::push(I_Object* obj) {
  LOGTRACEFN_IN;

  dtpInFifo.push(obj);
}

I_Object* NoAnalyzerDataPath::pop(void) {
  LOGTRACEFN_IN;

  // This is unnecesary. It's only for debugging purpouses
  MuonPath *ptrMp = (MuonPath*) candidateMuonPathFifo.pop();

  if (ptrMp != NULL) {
    LOGDEBUG("---");
    LOGDEBUG("*************************************");
    LOGDEBUG(
      NString("Generado MuonPath candidate. Capa/canal: ").\
        nAppend("%i", ptrMp->getPrimitive(0)->getLayerId()).sAppend("/").\
        nAppend("%i", ptrMp->getPrimitive(0)->getChannelId()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(1)->getLayerId()).sAppend("/").\
        nAppend("%i", ptrMp->getPrimitive(1)->getChannelId()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(2)->getLayerId()).sAppend("/").\
        nAppend("%i", ptrMp->getPrimitive(2)->getChannelId()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(3)->getLayerId()).sAppend("/").\
        nAppend("%i", ptrMp->getPrimitive(3)->getChannelId()).sAppend("\n").\
        sAppend("\nTDC Time's: ").\
        nAppend("%i", ptrMp->getPrimitive(0)->getTDCTime()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(1)->getTDCTime()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(2)->getTDCTime()).sAppend(" ").\
        nAppend("%i", ptrMp->getPrimitive(3)->getTDCTime()).sAppend(" "));
  }

  return ptrMp;
}

void NoAnalyzerDataPath::splitData(void) { chSplitter.splitData(); }
void NoAnalyzerDataPath::mixData(void) { dataMixer.mixData(); }

void NoAnalyzerDataPath::start(void) {
  thDataMix   = new Thread(1);
  thChSplit   = new Thread(2);
  thTimeWDisc = new Thread(3);

  dataMixer.setRRobin(true);
  chSplitter.setRRobin(true);
  twDiscriminator.setRRobin(true);

  thDataMix->doThisJob(dataMixer);
  thChSplit->doThisJob(chSplitter);
  thTimeWDisc->doThisJob(twDiscriminator);
}

void NoAnalyzerDataPath::stop(void) {
  twDiscriminator.stop();
  chSplitter.stop();
  dataMixer.stop();

  reset();
  delete thDataMix;
  reset();
  delete thChSplit;
  reset();
  delete thTimeWDisc;
}

void NoAnalyzerDataPath::reset(void) {
  dtpInFifo.reset();
  twDiscriminator.reset();
  dtpTimeFilteredFifo.reset();

  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c]->reset();

  dataMixer.reset();
  candidateMuonPathFifo.reset();
  chSplitter.resetStats();
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void NoAnalyzerDataPath::setMixerForcedTimeIncrement(int time) {
  dataMixer.setForcedTimeIncrement(time);
}

void NoAnalyzerDataPath::setDiscriminationTimeWindow(int tWindow) {
  twDiscriminator.setRejectTimeWindow(tWindow);
}

int NoAnalyzerDataPath::getDiscriminationTimeWindow(void) {
  return twDiscriminator.getRejectTimeWindow();
}

void NoAnalyzerDataPath::setAcceptanceTimeWindow(int tWindow) {
  twDiscriminator.setTimeWindow(tWindow);
}

int NoAnalyzerDataPath::getAcceptanceTimeWindow(void) {
  return twDiscriminator.getTimeWindow();
}

void NoAnalyzerDataPath::setTimeBufferTWindow(int tWindow) {
  for(int l = 0; l < NUM_LAYERS; l++)
    for(int c = 0; c < NUM_CH_PER_LAYER; c++)
      splittedChannels[l][c]->setTimeWindow(tWindow);

  dataMixer.setSeekTimeBufWindowRelBeginning(tWindow);
}

int NoAnalyzerDataPath::getTimeBufferTWindow(void) {
  return splittedChannels[0][0]->getTimeWindow();
}

}
