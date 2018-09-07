/**
 * Project:
 * Subproject: tracrecons
 * File name:  channelsplitter.h
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
#ifndef CHANNELSPLITTER_H
#define CHANNELSPLITTER_H

#include "analtypedefs.h"
#include "i_task.h"
#include "i_pusher.h"
#include "i_popper.h"

using namespace CIEMAT;

namespace CMS {

/**
 * Separa las DTPrimitives, procedentes de la FIFO de entrada, en buffers
 * individuales por canal.
 */
class ChannelSplitter : public I_Task {

  public:
    typedef struct {
      unsigned int numPrimitives[NUM_LAYERS][NUM_CH_PER_LAYER];
    } STATISTICS;

    ChannelSplitter(I_Popper& dataIn, midBuffers_t &channelOut);

    virtual ~ChannelSplitter();

    void run(void);
    void stop(void);
    void setRRobin(bool rr);
    void splitData(void);
    void resetStats(void);

    void setOutChannels(midBuffers_t &channelOut);
    const STATISTICS* getStatistics(void);

  private:
    I_Popper& dataIn;
    midBuffers_t channelOut;
    bool roundRobin;

    /* Contadores para estadísticas */
    STATISTICS stats;

    void resetOutChannels(void);
};

}

#endif
