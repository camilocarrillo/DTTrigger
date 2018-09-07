/**
 * Project:
 * Subproject: tracrecons
 * File name:  noanalyzerdatapath.h
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
 * Copyright (c) 2016-07-14 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef NOANALYZERDATAPATH_H
#define NOANALYZERDATAPATH_H

#include "i_pusher.h"
#include "i_popper.h"
#include "basicfifo.h"
#include "constants.h"
#include "timewindowdiscriminator.h"
#include "seektimebuffer.h"
#include "channelsplitter.h"
#include "multichdatamixer.h"
#include "thread.h"

using namespace CIEMAT;

namespace CMS {

/**
 * This class defines a component only for testing.
 * Basically it's the same as the "GlobalDataPath", but excluding the analyzer.
 * It mixes "DTPrims" into "MuonPaths", and dumps them into the outgoing file.
 * This way, we can analyze the quality of the mixes done by the first stage
 * of the "GlobalDataPath" component
 */

class NoAnalyzerDataPath : public I_Pusher, public I_Popper {

  public:
    NoAnalyzerDataPath(unsigned int maxMidFifoSize = 0);
    virtual ~NoAnalyzerDataPath();

    void push(I_Object* obj);
    I_Object* pop(void);

    /* Métodos para pruebas y depuración */
    void splitData(void);
    void mixData(void);
    void analyze(void);
    void start(void);
    void stop(void);

    void reset(void);

    void setMixerForcedTimeIncrement(int time);

    void setDiscriminationTimeWindow(int tWindow);
    int getDiscriminationTimeWindow(void);

    void setAcceptanceTimeWindow(int tWindow);
    int getAcceptanceTimeWindow(void);

    void setTimeBufferTWindow(int tWindow);
    int getTimeBufferTWindow(void);

  private:
    /*  Components */
    BasicFifo               dtpInFifo;
    BasicFifo               dtpTimeFilteredFifo;
    TimeWindowDiscriminator twDiscriminator;
    midBuffers_t            splittedChannels;
    ChannelSplitter         chSplitter;
    BasicFifo               candidateMuonPathFifo;
    MultiChDataMixer        dataMixer;

    /* Threads for parallel component processing */
    Thread *thTimeWDisc, *thChSplit, *thDataMix;
};

}

#endif
