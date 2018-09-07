/**
 * Project:
 * Subproject: tracrecons
 * File name:  segmentmatcher.h
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
#ifndef SEGMENT_MATCHER_H
#define SEGMENT_MATCHER_H

#include "i_task.h"
#include "i_pusher.h"
#include "constants.h"
#include "qslfilterpath.h"

using namespace CIEMAT;

namespace CMS {

/**
 * Separa los segmentos por superlayer
 */
class SegmentMatcher : public I_Task {

  public:
    SegmentMatcher(I_Pusher &dataOut);
    virtual ~SegmentMatcher();

    void run(void);
    void stop(void);
    void setRRobin(bool rr);
    void matchSegments(void);

    void setInChannels(QSLFilterPath *qslFilterIn[3]);

  private:
    QSLFilterPath *qslFilterIn[3];
    I_Pusher &dataOut;

    bool roundRobin;

    void resetInChannels(void);
};

}

#endif
