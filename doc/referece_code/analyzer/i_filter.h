/**
 * Project:
 * Subproject: tracrecons
 * File name:  i_filter.h
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
 * Copyright (c) 2015-07-02 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef I_FILTER_H
#define I_FILTER_H

#include "i_task.h"
#include "i_pusher.h"
#include "i_popper.h"

using namespace CIEMAT;

namespace CMS {

class I_Filter : public I_Task {

  public:
    I_Filter(I_Popper &dataIn, I_Pusher &dataOut);
    virtual ~I_Filter();

    void run(void);
    virtual void stop(void);
    void setRRobin(bool rr);
    virtual void filter(void)=0;

  protected:
    I_Popper &dataIn;
    I_Pusher &dataOut;

  private:
    bool roundRobin;
};

}

#endif
