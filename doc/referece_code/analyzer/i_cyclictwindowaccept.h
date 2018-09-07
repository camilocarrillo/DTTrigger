/**
 * Project:
 * Subproject: tracrecons
 * File name:  i_cyclictwindowaccept.h
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
#ifndef I_CYCLICTWINDOWACCEPT_H
#define I_CYCLICTWINDOWACCEPT_H

#include <pthread.h>

namespace CMS {

class I_CyclicTWindowAccept {

  public:
    I_CyclicTWindowAccept();
    virtual ~I_CyclicTWindowAccept();

    void setTimeWindow(int tWindow);
    int getTimeWindow(void);

    virtual void setCurrentTime(int time);
    int getCurrentTime(void);

    void reset(void);

  protected:
    /*
      This parameter will be used to determine if a new incoming DTPrimitive
      will be accepted for processing (filtering and accepting/rejecting) or
      will be kept pending until its TDCtime value is within the time-window
      starting on "currentTime" and ending on "currentTime + acceptTimeWindow"

      The value of this time windows and the currentTime increment rate should
      be calibrated to allow this thread to fetch enough DTP's to feed the rest
      of the processes, but few enough (retained in the incoming FIFO) to allow
      the processing of those sent previously.
    */
    int currentTime;
    bool isTimeWithinTheWindow(int time);

  private:
    pthread_mutex_t mtx;
    int timeWindow;
};

}
#endif
