/**
 * Project:
 * Subproject: tracrecons
 * File name:  i_cyclictwindowaccept.cpp
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
 * Copyright (c) 2016-11-30 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "constants.h"
#include "i_cyclictwindowaccept.h"

namespace CMS {

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
I_CyclicTWindowAccept::I_CyclicTWindowAccept() {
  pthread_mutex_init(&mtx, NULL);
  reset();
}

I_CyclicTWindowAccept::~I_CyclicTWindowAccept() {
  pthread_mutex_destroy(&mtx);
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void I_CyclicTWindowAccept::setTimeWindow(int tWindow) {
  pthread_mutex_lock(&mtx);

  if (tWindow > 0) timeWindow = tWindow;

  pthread_mutex_unlock(&mtx);
}

int I_CyclicTWindowAccept::getTimeWindow(void) { return timeWindow; }

void I_CyclicTWindowAccept::setCurrentTime(int time) {
  pthread_mutex_lock(&mtx);

  if (time >= 0) currentTime = time;
  else           currentTime = MAX_VALUE_OF_TIME + time + 1;
  
  pthread_mutex_unlock(&mtx);
}
int I_CyclicTWindowAccept::getCurrentTime(void) { return currentTime; }

void I_CyclicTWindowAccept::reset(void) {
  pthread_mutex_lock(&mtx);
  /*
    To avoid failures of some sibling classes because the mechanism for
    currentTime changing is not implemented (i.e. in the case of partial
    compilation by preprocessor macros to test only the analyzer), the default
    timeWindow is set at maximum, so those classes which call this method for
    time acceptance will not reject any data
  */
  timeWindow  = MAX_VALUE_OF_TIME;
  currentTime = 0;
  pthread_mutex_unlock(&mtx);
}

//------------------------------------------------------------------
//--- Métodos protegidos
//------------------------------------------------------------------
/**
 * This method checks if the (TDC) time passed as parameter is within the
 * defined time-window from the current time. It also takes into account that
 * the time is cyclical.
 */
bool I_CyclicTWindowAccept::isTimeWithinTheWindow(int time) {

  pthread_mutex_lock(&mtx);

  bool answer = false;
  int endOfTimeWindow = currentTime + timeWindow;

  if (endOfTimeWindow > MAX_VALUE_OF_TIME) {
    answer =
      (time >= currentTime && time <= MAX_VALUE_OF_TIME) ||
      (time >= 0           && time < endOfTimeWindow - MAX_VALUE_OF_TIME);
  }
  else answer = (time >= currentTime && time <= endOfTimeWindow);

  pthread_mutex_unlock(&mtx);

  return answer;
}

}
