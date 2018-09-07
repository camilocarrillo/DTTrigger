/**
 * Project:
 * Subproject: tracrecons
 * File name:  mpredundantfilter.h
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
 * Copyright (c) 2018-03-06 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef MPREDUNDANTFILTER_H
#define MPREDUNDANTFILTER_H

#include "analtypedefs.h"
#include "i_filter.h"
#include "muonpath.h"
#include "ringbuffer.h"

using namespace CIEMAT;

namespace CMS {
/*
 * Esta clase implementa un filtro que elimina elementos MuonPath duplicados.
 * 
 * Se basa en mantener un buffer con los últimos (64) elementos --distintos 
 * entre sí-- localizados y usarlos para comparar los nuevos elementos que
 * se van extrayendo de la fifo de entrada. Si un nuevo elemento extraído
 * es igual a alguno en el buffer, dicho elemento no es enviado a la fifo de
 * salida y es descartado
 * Si el elemento localizado no se encuentra en el buffer es insertado en él
 * como nuevo MP para comparaciones futuras, y una copia del mismo es enviada
 * a la salida.
 * Si el buffer está lleno, antes de insertar el nuevo elemento el más antiguo
 * es eliminado.
 */
class MPRedundantFilter : public I_Filter {

  public:
    MPRedundantFilter(I_Popper &dataIn, I_Pusher &dataOut, int bufSize = 64);
    virtual ~MPRedundantFilter();

    void filter(void);
    void reset(void);

  private:
    /*
     * En este buffer se mantienen los ultimos "bufSize" elementos procesados
     * (se mantienen copias de los mismos), de tal manera que sirven de 
     * elementos de comparacion para eliminar aquellos que, entrando por el 
     * "dataIn", sean identicos a algunos de los almacenados
     */
    RingBuffer buffer;
    
    bool isInBuffer(MuonPath* mPath);
};

}

#endif
