/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  globaldatapath.h
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
#ifndef GLOBALDATAPATH_H
#define GLOBALDATAPATH_H

#include "analtypedefs.h"
#include "basicfifo.h"
#include "channelsplitter.h"
#include "constants.h"
#include "i_popper.h"
#include "i_pusher.h"
#include "mpredundantfilter.h"
#include "multichdatamixer.h"
#include "pathanalyzer.h"
#include "qualityenhancerfilter.h"
#include "seektimebuffer.h"
#include "timewindowdiscriminator.h"
#include "thread.h"

#include "postanalisispath.h"

using namespace CIEMAT;

namespace CMS {
/**
 * Clase que representa un canal de análisis de datos.
 * Recibe cuatro entradas, correspondientes a 4 celdas (una por capa), y que
 * están bajo una de las posibles trayectorias del muón.
 * La entrada de índice 0 se corresponde con la capa inferior. Los siguientes
 * índices se suceden en orden hasta el 3 que se corresponde con el de la capa
 * exterior.
 * Este orden es crítico para el análisis que realizan los componentes de la
 * clase.
 */
class GlobalDataPath : public I_Pusher, public I_Popper {

  typedef struct {
    TimeWindowDiscriminator::STATISTICS twdStats;
    ChannelSplitter::STATISTICS cspStats;
  } STATISTICS;

  public:
    /* "cellLayout" representa el desplazamiento relativo (con signo) de la
       celda de cada capa, respecto de la celda base (que siempre tendrá
       desplazamiento 0), medido en unidades de semilongitud de celda */
    GlobalDataPath(unsigned int maxMidFifoSize = 0);
    virtual ~GlobalDataPath();

    void push(I_Object* obj);
    I_Object* pop(void);

    /* Métodos para pruebas y depuración */
    void discriminate(void);
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

    void setPathTolerance(int tolerance);
    int getPathTolerance(void);

    void setBXTolerance(int tolerance);
    int getBXTolerance(void);

    void setMinMuonPQuality(MP_QUALITY q);
    MP_QUALITY getMinMuonPQuality(void);

    void setChiSquareThreshold(float ch2Thr);
    
    const STATISTICS* getStatistics(void);

    unsigned int getNumCandidateMuonPaths(void);
    unsigned int getNumValidMuonPaths(void);
    /*
     * Métodos chapuzas para poder garantizar que no hay datos en proceso
     * de un determinado evento antes de inyectar los datos del evento 
     * siguiente.
     * 
     * Esto sólo es útil para las simulaciones. Para el sistema final habrá que
     * estudiar muy bien la temporización.
     */
    bool isThereDataInAnyMiddleChannel(void);
    bool areEmptyPreviousBuffers(void);
    bool areEmptySegmentBuffers(void);

  private:
    STATISTICS stats;

    /*  Components */
    BasicFifo               dtpInFifo;
    BasicFifo               dtpTimeFilteredFifo;
    TimeWindowDiscriminator twDiscriminator;
    midBuffers_t            splittedChannels;
    ChannelSplitter         chSplitter;
    BasicFifo               candidateMuonPathFifo;
    MultiChDataMixer        dataMixer;
    BasicFifo               validMuonPathFifo;
    PathAnalyzer            dataAnalyzer;
    BasicFifo               filteredValidMPFifo;
    MPRedundantFilter       dupMPValidFilter;
    
    PostAnalisisPath        postAnPath;

    /* Threads for parallel component processing */
    Thread *thTimeWDisc, *thChSplit, *thDataMix, *thDataAnal, *thValMPFilter;
};

}

#endif
