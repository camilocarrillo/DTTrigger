/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  simpledatapath.h
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
#ifndef SIMPLEDATAPATH_H
#define SIMPLEDATAPATH_H

#include "analtypedefs.h"
#include "i_pusher.h"
#include "i_popper.h"
#include "basicfifo.h"
#include "pathanalyzer.h"
#include "simplemuonpathbuilder.h"
#include "thread.h"

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
class SimpleDataPath : public I_Pusher, public I_Popper {

  public:
    SimpleDataPath(unsigned int maxMidFifoSize = 0);
    virtual ~SimpleDataPath();

    void push(I_Object* obj);
    I_Object* pop(void);

    /* Métodos para pruebas y depuración */
    void splitData(void);
    void buildData(void);
    void analyze(void);
    void start(void);
    void stop(void);

    void reset(void);

    void setPathTolerance(int tolerance);
    int getPathTolerance(void);

    void setBXTolerance(int tolerance);
    int getBXTolerance(void);

    void setMinMuonPQuality(MP_QUALITY q);
    MP_QUALITY getMinMuonPQuality(void);

  private:
    /*  Components */
    BasicFifo             dtpInFifo;
    BasicFifo             candidateMuonPathFifo;
    SimpleMuonPathBuilder mpBuilder;
    BasicFifo             validMuonPathFifo;
    PathAnalyzer          dataAnalyzer;

    /* Threads for parallel component processing */
    Thread *thMPBuild, *thDataAnal;
};

}

#endif
