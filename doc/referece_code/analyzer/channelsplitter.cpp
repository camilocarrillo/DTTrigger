/**
 * Project:
 * Subproject: tracrecons
 * File name:  channelsplitter.cpp
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
#include "channelsplitter.h"
#include "dtprimitive.h"
#include "logmacros.h"

extern bool orbitDataInserted;
extern bool orbitDataSplitted;

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
ChannelSplitter::ChannelSplitter(I_Popper& dataIn, midBuffers_t &channelOut)
  : I_Task(), dataIn(dataIn)
{
  LOGTRACE("Creando un 'ChannelSplitter'");
  setOutChannels(channelOut);
  setRRobin(false);
  resetStats();
}

ChannelSplitter::~ChannelSplitter() {
  LOGTRACE("Destruyendo un 'ChannelSplitter'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void ChannelSplitter::run(void)
{
  LOGTRACEFN_IN;

  while(roundRobin)
    splitData();

  LOGTRACEFN_OUT;
}

void ChannelSplitter::stop(void) {
  setRRobin(false);
  dataIn.reset();
  resetStats();
  resetOutChannels();
}

void ChannelSplitter::setRRobin(bool rr) { roundRobin = rr; }

void ChannelSplitter::splitData(void) {

//*********************************** 
if (orbitDataInserted && !orbitDataSplitted) { 
  
  DTPrimitive* dtpAux = (DTPrimitive*) dataIn.pop();

  /* To avoid NULL pointer after a 'stop' command */
  if (dtpAux == NULL) return;

  int channel = dtpAux->getChannelId();
  int layer   = dtpAux->getLayerId();

  /*
   * NOTA: No se hacen cambios en el ChannelSplitter para la versión 
   *       MULTILAYER porque, para ahorrar recursos, se van a tratar todas
   *       las celdas de igual índice de wire, de todas la layers, 
   *       como si de una única celda se tratara. 
   *       Será el MIXER el encargado de mezclar solamente hits 
   *       correspondientes a una superlayer dada.
   *       De esta forma se ahorrarían muchos buffers y mucha lógica adicional
   */
  (channelOut[layer][channel])->push(dtpAux);
  stats.numPrimitives[layer][channel]++;
  
  if (dataIn.empty()) orbitDataSplitted = true;
  
} //***********************************

}

void ChannelSplitter::resetStats(void) {
  for(int lay = 0; lay < NUM_LAYERS; lay++)
    for(int ch = 0; ch < NUM_CH_PER_LAYER; ch++)
      stats.numPrimitives[lay][ch] = 0;
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void ChannelSplitter::setOutChannels(midBuffers_t &channelOut)
{
  LOGTRACEFN_IN;

  for (int lay = 0; lay < NUM_LAYERS; lay++)
    for (int ch = 0; ch < NUM_CH_PER_LAYER; ch++)
      this->channelOut[lay][ch] = channelOut[lay][ch];
}

const ChannelSplitter::STATISTICS* ChannelSplitter::getStatistics(void) {
  return &stats;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void ChannelSplitter::resetOutChannels(void) {
  for(int lay = 0; lay < NUM_LAYERS; lay++)
    for(int ch = 0; ch < NUM_CH_PER_LAYER; ch++)
      (this->channelOut[lay][ch])->reset();
}

}
