/**
 * Project:
 * Subproject: tracrecons
 * File name:  qualityenhancerfilter.cpp
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
 * Copyright (c) 2018-04-04 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "qualityenhancerfilter.h"
#include "logmacros.h"
#include "muonpath.h"

namespace CMS {

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
QualityEnhancerFilter::QualityEnhancerFilter(I_Popper &dataIn, 
                                             I_Pusher &dataOut,
                                             int bufSize) :
  I_Filter(dataIn, dataOut),
  buffer(bufSize)
{
  LOGTRACE("Creando un 'QualityEnhancerFilter'");
  /* 
   * Los datos que se envían, puesto que no se duplican, no han de ser 
   * eliminados. En el caso que hayan de borrarse de la memoria, el método
   * "cleanBuffer" se encarga de ello y habrá de ser invocado "a mano" 
   * en cada punto del código que sea necesario.
   */
  buffer.deleteObjsOnForget(false);
  reset();
}

QualityEnhancerFilter::~QualityEnhancerFilter()
{
  LOGTRACE("Destruyendo un 'QualityEnhancerFilter'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void QualityEnhancerFilter::filter(void) {

  MuonPath *mPath = (MuonPath*) dataIn.pop();
  /*
    Esta línea se incluye para evitar que, tras un 'stop', que fuerza la
    liberación del mutex de la fifo de entrada, devuelva un puntero nulo.
   */
  if (mPath == NULL) return;

  // En caso que no esté en el buffer, será enviado al exterior.
  checkNewData(mPath);

  if ( sendStoredValue ) {
    sendStoredValue = false;
    /* 
     * Enviamos el elemento almacenado que se consideró el mejor, y en caso
     * que sea de baja calidad (1 o 2), los compañeros de también baja calidad
     * almacenados en el buffer temporal también se envían.
     */
    if (currentMPath->getQuality() <= LOWQ) sendDataInBuffer();
    else                                    cleanBuffer();

    dataOut.push( currentMPath );
    // Reemplazamos por el nuevo evento disjunto que se ha detectado.
    currentMPath = mPath;
  }
}

void QualityEnhancerFilter::reset(void) { 
  currentMPath    = NULL;
  sendStoredValue = false;
}

/**
 * Se sobrecarga el método "stop" de la clase padre para que durante la 
 * parada del procesamiento se fuerce el envío del último elemento almacenado
 * en "currentMPath".
 * 
 * En el sistema real no tiene sentido porque siempre hay una inyección de datos
 * en la fifo de entrada, pero incluso ahí habrá que implementar algo para
 * forzar el envío del dato que quede pendiente de envío entre evento y
 * evento.
 * 
 * ¡¡¡ HAY QUE PENSAR ESTO EN DETALLE A LA HORA DE PORTAR A FPGA !!!!
 */
void QualityEnhancerFilter::stop(void) {

  if (currentMPath != NULL) {
    if (currentMPath->getQuality() <= LOWQ) sendDataInBuffer();
    else                                    cleanBuffer();

    dataOut.push( currentMPath );
  }

  I_Filter::stop();
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void QualityEnhancerFilter::checkNewData(MuonPath* mPath) {
  
  // Primer ciclo
  if (currentMPath == NULL) {
    currentMPath = mPath;
    return;
  }
  /*
   * Cuando comparten algún elemento en común, implica que son de la misma
   * "familia" y por tanto se analizan y comparan en función de sus calidades
   * y su chi-square.
   * 
   * Aquí se asume que los elementos de las "familias" vienen consecutivas
   * en el buffer de entrada, lo que es razonable dada la forma de generación
   * de los segmentos y subsegmentos. Esto permite hacer un filtro 
   * razonablemente sencillo.
   * Si esta hipótesis no fuera válida, habría que reescribir este filtro
   * o añadir otro a continuación del pipeline (mejor esto último) para 
   * completar la limpieza de elementos.
   */
  if (isSimilar2Current(mPath)) {

    if (currentMPath->getQuality() >= HIGHQGHOST) {
      if (mPath->getQuality() >= HIGHQGHOST && 
          mPath->getChiSq()   < currentMPath->getChiSq())
      {
        currentMPath = mPath;
      }
    }
    else {
      buffer.push(currentMPath);
      currentMPath = mPath;
    }
  }
  else {
    /*
      * If they are not "similar" it means that a new element from a new 
      * "family" has come, so old one has to be sent and this new one
      * has to be considered as the new element for future checkings.
      */
    sendStoredValue = true;
  }
}

/**
 * Returns true when current element and value passed share in common some
 * wire ID's and some TDC values.
 */
bool QualityEnhancerFilter::isSimilar2Current(MuonPath* mPath) {
  int countCoincidences = 0;

  for (int i = 0; i <= 3; i++) 
    if (mPath->getPrimitive(i)->getTDCTime() == 
        currentMPath->getPrimitive(i)->getTDCTime()   &&

        mPath->getPrimitive(i)->getSuperLayerId() == 
        currentMPath->getPrimitive(i)->getSuperLayerId() &&

        mPath->getPrimitive(i)->getChannelId() == 
        currentMPath->getPrimitive(i)->getChannelId() &&

        currentMPath->getPrimitive(i)->isValidTime()  &&
        mPath->getPrimitive(i)->isValidTime()) countCoincidences++;
  
  return (countCoincidences > 0);
}

/**
 * This method sends to the outgoing FIFO all data stored in the (low quality)
 * buffer.
 * After that it deletes the buffer contents, but DO NOT DELETE THE ELEMENTS.
 */
void QualityEnhancerFilter::sendDataInBuffer(void) {
  
  if ( !buffer.isEmpty() ) {
    // Send data
    int numElements = buffer.storedElements();
    for (int i = 0; i < numElements; i++) 
      dataOut.push( (MuonPath*) buffer.readAt(i) );
    
    // Delete contents but not elements (see "constructor")
    buffer.forgetData( numElements );
  }
}

/**
 * Borra el contenido del buffer interno, borrando también los elementos 
 * que hay en él.
 */
void QualityEnhancerFilter::cleanBuffer(void) {
  if ( !buffer.isEmpty() ) {

    buffer.deleteObjsOnForget(true);
    buffer.forgetData( buffer.storedElements() );
    buffer.deleteObjsOnForget(false);

  }
}

}
