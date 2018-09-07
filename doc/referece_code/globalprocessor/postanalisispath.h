/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  postanalisispath.h
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
#ifndef POSTANALISYSPATH_H
#define POSTANALISYSPATH_H

#include "basicfifo.h"
#include "i_popper.h"
#include "thread.h"
#include "qslfilterpath.h"
#include "suplaysplitter.h"
#include "segmentmatcher.h"

using namespace CIEMAT;

namespace CMS {

class PostAnalisisPath : public I_Popper {

  public:
    PostAnalisisPath(I_Popper& dataIn, unsigned int maxMidFifoSize = 32);
    virtual ~PostAnalisisPath();

    I_Object* pop(void);

    void start(void);
    void stop(void);

    void reset(void);
    bool empty(void);

  private:
    QSLFilterPath    qslPathLayerPHIL, qslPathLayerTHETA, qslPathLayerPHIH;
    QSLFilterPath    *qslLayers[3];
    SupLayerSplitter slSplitter;
    BasicFifo        outSegmentFifo;
    SegmentMatcher   sgMatch;

    /* Threads for parallel component processing */
    Thread *thSLSplitter, *thSegmentMatch;
};

}

#endif
