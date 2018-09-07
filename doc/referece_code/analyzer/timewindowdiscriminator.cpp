/**
 * Project:
 * Subproject: tracrecons
 * File name:  timewindowdiscriminator.cpp
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
 * Copyright (c) 2015-04-15 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "logmacros.h"
#include "timewindowdiscriminator.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
TimeWindowDiscriminator::TimeWindowDiscriminator(I_Popper &dataIn,
                                                 I_Pusher &dataOut) :
  I_Filter(dataIn, dataOut), I_CyclicTWindowAccept()
{
  LOGTRACE("Creando un 'TimeWindowDiscriminator'");
  reset();
}

TimeWindowDiscriminator::~TimeWindowDiscriminator() {
  LOGTRACE("Destruyendo un 'TimeWindowDiscriminator'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
/*
  In the mean time, this method is splitted from previous one in order to
  be called from a upper hierarchy component or to do some test.
  Building it as "inline" allows the same processing speed as if it was
  embedded into 'run' method and improved code readness
 */
void TimeWindowDiscriminator::filter(void){

  if ( !thereIsPendingData ) dtPrim = (DTPrimitive*) dataIn.pop();
  /*
    Putting this line here, I save a lot of (equal) sentences, along the next
    chain of "if" that are mandatory to reset to false this attribute.
  */
  thereIsPendingData = false;
  /*
    Esta línea se incluye para evitar que, tras un 'stop', que fuerza la
    liberación del mutex de la fifo de entrada, devuelva un puntero nulo, lo que
    a su vez, induce un error en la ejecución al intentar acceder a cualquiera
    de los métodos de la clase 'DTPrimitive'
   */
  if (dtPrim == NULL) return;

  int supLay  = dtPrim->getSuperLayerId();
  int layer   = dtPrim->getLayerId();
  int channel = dtPrim->getChannelId();
  int tdcTime = dtPrim->getTDCTime();
  int orbit   = dtPrim->getOrbit();

  LOGDEBUG_INT("TWD.discriminate. Obtenida DTPrimitive con TDCT: ",
                dtPrim->getTDCTime());
  LOGDEBUG_UINT("Orbit: ", orbit);
  LOGDEBUG_UINT("SuperL: ", supLay);
  LOGDEBUG_UINT("Layer: ", layer);
  LOGDEBUG_UINT("Channel: ", channel);

  if (channel < NUM_CH_PER_LAYER && layer < NUM_LAYERS) {
    /*
      Cada nuevo valor de TDC ha de ser superior al anterior mas una cierta
      ventana de discriminación, para evitar la saturación del analizador, y
      porque la recepción de hits demasiado contínuos, con separaciones
      inferiores al tiempo de latencia de la cámara, será debida a señales
      espúreas.
      Cada nuevo valor de TDC aceptado actúa como umbral temporal para el
      siguiente.
      A su vez, debido al "rollover" del valor temporal de los TDC's (el tiempo
      es circular), si un cierto valor es inferior al último valor de TDC
      aceptado, implica que se ha producido el "rollover" y los nuevos valores
      de TDC tienen que volverse a aceptar. Esto es así porque, para cada canal,
      se asume que los valores de TDC vienen ordenados temporalmente.
    */
    if (tdcTime >= lastTimeStamp[supLay][layer][channel] + rejectTimeWindow ||
        tdcTime <= lastTimeStamp[supLay][layer][channel])
    {
      /*
        If "tdcTime" is in the acceptance window, we send it to the next
        level. If not, it will kept pending until rolling time window reaches
        it. Meanwhile, no other primitives will be fetched from the incoming
        FIFO, neither others will be sent to the outgoing one.
      */
      if ( isTimeWithinTheWindow(tdcTime) ) {
        lastTimeStamp[supLay][layer][channel] = tdcTime;
        dataOut.push(dtPrim);

        stats.accepted[supLay][layer][channel]++;
        stats.totAccepted++;
        LOGDEBUG("TWD.discriminate. DTP aceptada.");
      }
      else thereIsPendingData = true;
    }
    else {
      delete dtPrim;
      stats.totDiscarded++;
      stats.discarded[supLay][layer][channel]++;
      LOGDEBUG("TWD.discriminate. DTP rechazada.");
    }
  }
  else {
    delete dtPrim;
    stats.totInvalid++;
    LOGDEBUG("TWD.discriminate. DTP INVALIDA.");
  }
}

void TimeWindowDiscriminator::reset(void) {

  rejectTimeWindow   = 0;
  thereIsPendingData = false;
  dtPrim             = NULL;

  I_CyclicTWindowAccept::reset();

  stats.totAccepted  = 0;
  stats.totDiscarded = 0;
  stats.totInvalid   = 0;

  for(int supLay = 0; supLay < NUM_SUPERLAYERS; supLay++)
    for(int lay = 0; lay < NUM_LAYERS; lay++)
      for(int ch = 0; ch < NUM_CH_PER_LAYER; ch++) {
        lastTimeStamp[supLay][lay][ch]   = 0;
        stats.accepted[supLay][lay][ch]  = 0;
        stats.discarded[supLay][lay][ch] = 0;
      }
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
/**
 * In order to optimize comparation algorithm, being as when new DT values are
 * compared against last reference time-stamp it's neccesary to increase this
 * value by 'tWindow', it's better to add directly this 'tWindow' value to
 * every reference time-stamp, avoiding many sums while processing new data.
 * This is useful specially when we'll develop the algorithm variant oriented
 * to FPGA.
 */
void TimeWindowDiscriminator::setRejectTimeWindow(int tWindow) {
  if (tWindow >= 0) rejectTimeWindow = tWindow;
}

int TimeWindowDiscriminator::getRejectTimeWindow(void) {
  return rejectTimeWindow;
}

const TimeWindowDiscriminator::STATISTICS*
    TimeWindowDiscriminator::getStatistics(void) { return &stats; }

}
