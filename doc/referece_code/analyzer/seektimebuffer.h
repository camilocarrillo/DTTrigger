/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  seektimebuffer.h
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
#ifndef SEEKTIMEBUFFER_H
#define SEEKTIMEBUFFER_H

#include "constants.h"
#include "i_popper.h"
#include "i_pusher.h"
#include "ringbuffer.h"
#include "i_cyclictwindowaccept.h"

using namespace CIEMAT;

namespace CMS {

class SeekTimeBuffer : public I_Popper, public I_Pusher,
                       public I_CyclicTWindowAccept
{
  public:
    SeekTimeBuffer(unsigned int maxSize = 2, int layId = 0, int chId = 0)
      throw (CException);
    virtual ~SeekTimeBuffer();

    I_Object* pop(void);
    void push(I_Object* obj);
    void reset(void);
    void rollBack(void);
    unsigned int storedElements(void);
    bool isEmpty(void);

    void setCurrentTime(int time);

  private:
    RingBuffer buffer;
    pthread_mutex_t mtx;
    unsigned int readPtr;

    int channelId;
    int layerId;

    void forgetData(int oldCurrentTime, int newCurrentTime);
};

}

#endif
