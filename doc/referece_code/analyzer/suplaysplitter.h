/**
 * Project:
 * Subproject: tracrecons
 * File name:  suplaysplitter.h
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
#ifndef SUPLAYER_SPLITTER_H
#define SUPLAYER_SPLITTER_H

#include "i_task.h"
#include "i_popper.h"
#include "constants.h"
#include "qslfilterpath.h"

using namespace CIEMAT;

namespace CMS {

/**
 * Separa los segmentos por superlayer
 */
class SupLayerSplitter : public I_Task {

  public:
    SupLayerSplitter(I_Popper& dataIn);

    virtual ~SupLayerSplitter();

    void run(void);
    void stop(void);
    void setRRobin(bool rr);
    void splitData(void);

    // Must be called before running
    void setOutChannels(QSLFilterPath *qslFilterOut[NUM_SUPERLAYERS]);

  private:
    I_Popper &dataIn;
    QSLFilterPath *qslFilterOut[NUM_SUPERLAYERS];

    bool roundRobin;

    void resetOutChannels(void);
};

}

#endif
