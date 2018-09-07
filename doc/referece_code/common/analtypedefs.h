/**
 * Project:
 * Subproject: tracrecons
 * File name:  analtypdefs.h
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
 * Copyright (c) 2015-02-10 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef ANALTYPDEFS_H
#define ANALTYPDEFS_H

#include "seektimebuffer.h"
#include "constants.h"

namespace CMS {
  /* Posibles calidades de la trayectoria:
     NOPATH => No es una trayectoria válida
     LOWQGHOST => Igual que LOWQ con múltiples casos simultáneos
     LOWQ   => Es una potencial trayectoria pero sólo formada por 3 puntos
     HIGHQGHOST => Igual que HIGHQ con múltiples casos simultáneos
     HIGHQ  => Es una trayectoria válida con 4 puntos alineados (4 celdas)
   */
  typedef enum {NOPATH = 0, LOWQGHOST, LOWQ, HIGHQGHOST, HIGHQ} MP_QUALITY;

  // Tipos de lateralidad de traza de partícula al pasar por una celda
  typedef enum {LEFT = 0, RIGHT} LATERAL_CASES;

  typedef SeekTimeBuffer* midBuffers_t[NUM_LAYERS][NUM_CH_PER_LAYER];
}

#endif
