/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  qslfilterpath.h
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
#ifndef QSLFILTERPATH_H
#define QSLFILTERPATH_H

#include "basicfifo.h"
#include "i_popper.h"
#include "i_pusher.h"
#include "thread.h"
#include "qualityenhancerfilter.h"

using namespace CIEMAT;

namespace CMS {

class QSLFilterPath : public I_Pusher, public I_Popper {

  public:
    QSLFilterPath(unsigned int maxMidFifoSize = 32);
    virtual ~QSLFilterPath();

    void push(I_Object* obj);
    I_Object* pop(void);

    void start(void);
    void stop(void);

    void reset(void);
    bool empty(void);

  private:
    /*  Components */
    BasicFifo               validSegmentFifo;
    BasicFifo               enhancedQualityMPFifoST1;
    QualityEnhancerFilter   eQualityMPFilterST1;
    BasicFifo               enhancedQualityMPFifoST2;
    QualityEnhancerFilter   eQualityMPFilterST2;
    
    /* Threads for parallel component processing */
    Thread *thEnQMPFilterST1, *thEnQMPFilterST2;
};

}

#endif
