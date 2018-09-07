/**
 * Project:
 * Subproject: tracrecons
 * File name:  qualityenhancerfilter.h
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
 * Copyright (c) 2018-04-04 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef MPQENHANCERFILTER_H
#define MPQENHANCERFILTER_H

#include "analtypedefs.h"
#include "i_filter.h"
#include "muonpath.h"
#include "ringbuffer.h"

using namespace CIEMAT;

namespace CMS {

class QualityEnhancerFilter : public I_Filter {

  public:
    QualityEnhancerFilter(I_Popper &dataIn, I_Pusher &dataOut, 
                          int bufSize = 32);

    virtual ~QualityEnhancerFilter();

    void filter(void);
    void reset(void);
    void stop(void);

  private:
    MuonPath* currentMPath;
    bool sendStoredValue;
    /*
     * Este buffer es sólo para almacenar temporalmente los datos de un
     * evento (los de calidades 2 y 1) en espera de comprobar si aparece uno
     * equivalente de alta calidad (3 o 4), en cuyo caso se descartan los 
     * de baja calidad, o enviar todos los detectados y almacenados en el
     * buffer.
     */
    RingBuffer buffer;

    void checkNewData(MuonPath* mPath);
    bool isSimilar2Current(MuonPath* mPath);
    void sendDataInBuffer(void);
    void cleanBuffer(void);
};

}

#endif
