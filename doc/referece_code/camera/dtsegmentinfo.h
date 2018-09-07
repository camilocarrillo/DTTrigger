/**
 * Project:
 * Subproject: tracrecons
 * File name:  dtsegmentinfo.h
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
 * Copyright (c) 2018 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef DTSEGMENTINFO_H
#define DTSEGMENTINFO_H

#include "analtypedefs.h"

using namespace CIEMAT;
using namespace CMS;

class DTSegmentInfo {

  public:
    DTSegmentInfo();
    DTSegmentInfo(DTSegmentInfo *ptr);
    virtual ~DTSegmentInfo();

    bool isValid(void);

    void setBxTimeValue(int time);
    int  getBxTimeValue(void);
    int  getBxNumId(void);

    void setLateralComb(LATERAL_CASES latComb[4]);
    void setLateralComb(const LATERAL_CASES *latComb);
    const LATERAL_CASES* getLateralComb(void);

    void  setHorizPos(float pos);
    float getHorizPos(void);

    void  setTanPhi(float tanPhi);
    float getTanPhi(void);

    void  setChiSq(float chi);
    float getChiSq(void);

    void  setXCoorCell(float x, int cell);
    float getXCoorCell(int cell);

    void  setDriftDistance(float dx, int cell);
    float getDriftDistance(int cell);

  private:
    /* Combinación de lateralidad */
    LATERAL_CASES lateralComb[4];
    /* Tiempo del BX respecto del BX0 de la órbita en curso */
    int bxTimeValue;
    /* Número del BX dentro de una órbita */
    int bxNumId;
    /* Parámetros de celda */
    float xCoorCell[4];         // Posicion horizontal del hit en la cámara
    float xDriftDistance[4];    // Distancia de deriva en la celda (sin signo)
    /* Parámetros de trayectoria */
    float tanPhi;
    float horizPos;
    /* Estimador de calidad de trayectoria */
    float chiSquare;
};

#endif
