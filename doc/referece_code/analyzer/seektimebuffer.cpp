/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  seektimebuffer.cpp
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
 * Copyright (c) 2015-03-04 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "seektimebuffer.h"
#include "dtprimitive.h"
#include "logmacros.h"

namespace CMS {
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
SeekTimeBuffer::SeekTimeBuffer(unsigned int maxSize, int layId, int chId)
  throw (CException) : I_Popper(), I_Pusher(), I_CyclicTWindowAccept(),
                       buffer(maxSize)
{
  LOGTRACE("Creando un 'SeekTimeBuffer'");

  /* To avoid internal-buffer data overwritting */
  buffer.setDataOverWritingMode(false);
  /* To force objects deletion at the same time that their
    pointers are removed */
  buffer.deleteObjsOnForget(true);

  pthread_mutex_init(&mtx, NULL);
  channelId = chId;
  layerId   = layId;
  reset();
}

SeekTimeBuffer::~SeekTimeBuffer() {
  LOGTRACE("Destruyendo un 'SeekTimeBuffer'");
  pthread_mutex_destroy(&mtx);
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
/**
 * This method returns a 'DTPrimitive' stored into the internal buffer which
 * 'tdcTime' is within the configured time window.
 *
 * Programmed time window is a value relative to some sort of "current time".
 *
 * If no one within that window is found, it returns a new dummy 'DTPrimitive'
 * with its time value invalidated (this is the default behaviour of a
 * 'DTPrimitive' object when created by its default constructor), in order to
 * avoid returning NULL objects which could give problems to the caller of this
 * method (mainly 'PathDataMixer' component).
 */
I_Object* SeekTimeBuffer::pop(void) {

  LOGTRACEFN_IN;

  DTPrimitive *objPtr = NULL;
  DTPrimitive *objAux = NULL;

  pthread_mutex_lock(&mtx);

  unsigned int numElements = buffer.storedElements();

  LOGTRACE(NString("Pop en 'SeekTimeBuffer'. Layer: ").\
           nAppend("%d", layerId).sAppend(", Channel: ").\
           nAppend("%d", channelId).sAppend(". Elementos: ").\
           nAppend("%d", numElements));

  if (readPtr >= numElements) readPtr = 0;

  if (readPtr < numElements) {
    // Pointers are relative to the begining of the internal RingBuffer.
    objAux = (DTPrimitive*) buffer.readAt(readPtr);
    int tdcTime = objAux->getTDCTime();
    /*
      If TDC time is within configured time-window, it's created an object's
      copy, and sent to this method caller
     */
    if ( isTimeWithinTheWindow(tdcTime) ) objPtr = new DTPrimitive( objAux );
    readPtr++;
  }

  pthread_mutex_unlock(&mtx);

  LOGTRACE(NString("Final de Pop en 'SeekTimeBuffer'. Layer: ").\
           nAppend("%d", layerId).sAppend(", Channel: ").\
           nAppend("%d", channelId));
  /*
    If we haven't found any valid DTPrimitive, we return a not NULL object,
    but with its TDC time value marked as not-valid (default behaviour)
   */
  if (objPtr == NULL) objPtr = new DTPrimitive();
  return objPtr;
}

/**
 * Inserts a new DTPrimitive in the buffer.
 * It has been modified in order to detect cases when is likely that a new
 * 'DTPrimitive' has a time-stamp that overflows BX Counter. In this case, this
 * new object must not be remove by the time-window exclusion mechanism,
 * implemented within 'pop' method
 */
void SeekTimeBuffer::push(I_Object* obj) {
  LOGTRACEFN_IN;

  buffer.push(obj);
}

void SeekTimeBuffer::reset(void) {
  LOGTRACEFN_IN;

  readPtr = 0;
  I_CyclicTWindowAccept::reset();
  I_CyclicTWindowAccept::setTimeWindow(600);

  buffer.reset();
}

void SeekTimeBuffer::rollBack(void) {
  pthread_mutex_lock(&mtx);
  readPtr = 0;
  pthread_mutex_unlock(&mtx);
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
unsigned int SeekTimeBuffer::storedElements(void) {
  return buffer.storedElements();
}

bool SeekTimeBuffer::isEmpty(void) { return buffer.isEmpty(); }

void SeekTimeBuffer::setCurrentTime(int time) {
  pthread_mutex_lock(&mtx);
  /*
    At the same time that new 'currentTime' is established, old elements outside
    the time new time-window are removed
  */
  forgetData(currentTime, time);
  I_CyclicTWindowAccept::setCurrentTime(time);

  pthread_mutex_unlock(&mtx);
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
/**
 * Removes elements which TDC time-stamp is older than the new current time.
 * It takes care of not removing those elements that satisfy this condition, but
 * have a smaller time due to BX time counters overflow.
 *
 * It is assumed that 'newCurrentTime' will be never greater than
 * 'MAX_VALUE_OF_TIME'
 */
void SeekTimeBuffer::forgetData(int oldCurrentTime, int newCurrentTime) {

  int numElements = buffer.storedElements();
  int tdcTime = 0, i = 0, j = 0;

  int lowerLimit = oldCurrentTime;
  /*
    These loops walk along the internal buffer to determine the last object
    which TDC time-stamp is lower than the new programmed current time.
    At the end, "j" contains the number of items that should be removed.
  */
  if (newCurrentTime < oldCurrentTime) {
    for (i = 0; i < numElements; i++) {
      tdcTime = ((DTPrimitive*) buffer.readAt(i))->getTDCTime();
      /* WARNING!!! This condition is NEGATED */
      if ( !(tdcTime >= oldCurrentTime && tdcTime <= MAX_VALUE_OF_TIME) ) break;
    }
    lowerLimit = 0;
  }
  /*
    NOTE: if previous condition is not satisfied, consequently previous loop
    is not executed , be aware that "i" is initialized to 0
  */
  for (j = i; j < numElements; j++) {
    tdcTime = ((DTPrimitive*) buffer.readAt(j))->getTDCTime();
    /* WARNING!!! This condition is NEGATED */
    if ( !(tdcTime >= lowerLimit && tdcTime <= newCurrentTime) ) break;
  }

  buffer.forgetData(j);
}

}
