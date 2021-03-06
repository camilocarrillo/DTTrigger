#ifndef MUONPATH_H
#define MUONPATH_H
#include <iostream> 
#include "analtypedefs.h"


#include "L1Trigger/DTTrigger/src/dtprimitive.h"

class MuonPath {

  public:
    MuonPath(DTPrimitive *ptrPrimitive[4]);
    MuonPath(MuonPath *ptr);
    virtual ~MuonPath();

    void setPrimitive(DTPrimitive *ptr, int layer);
    DTPrimitive *getPrimitive(int layer);

    void setCellHorizontalLayout(int layout[4]);
    void setCellHorizontalLayout(const int *layout);
    const int* getCellHorizontalLayout(void);

    int  getBaseChannelId(void);
    void setBaseChannelId(int bch);

    void setQuality(MP_QUALITY qty);
    MP_QUALITY getQuality(void);

    bool isEqualTo(MuonPath *ptr);
    
    /* El MuonPath debe ser analizado si hay al menos 3 Primitivas válidas */
    bool isAnalyzable(void);
    /* Indica que hay 4 Primitivas con dato válido */
    bool completeMP(void);

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
    //------------------------------------------------------------------
    //--- Datos del MuonPath
    //------------------------------------------------------------------
    /*
      Primitivas que forman el path. En posición 0 está el dato del canal de la
      capa inferior, y de ahí hacia arriba. El orden es crítico.
     */
    DTPrimitive *prim[4];

    /* Posiciones horizontales de cada celda (una por capa), en unidades de
       semilongitud de celda, relativas a la celda de la capa inferior
       (capa 0). Pese a que la celda de la capa 0 siempre está en posición
       0 respecto de sí misma, se incluye en el array para que el código que
       hace el procesamiento sea más homogéneo y sencillo.
       Estos parámetros se habían definido, en la versión muy preliminar del
       código, en el 'PathAnalyzer'. Ahora se trasladan al 'MuonPath' para
       que el 'PathAnalyzer' sea un único componente (y no uno por posible
       ruta, como en la versión original) y se puede disponer en arquitectura
       tipo pipe-line */
    int cellLayout[4];
    int baseChannelId;

    //------------------------------------------------------------------
    //--- Resultados tras cálculos
    //------------------------------------------------------------------
    /* Calidad del path */
    MP_QUALITY quality;
    
    /* Combinación de lateralidad */
    LATERAL_CASES lateralComb[4];

    /* Tiempo del BX respecto del BX0 de la órbita en curso */
    int bxTimeValue;

    /* Número del BX dentro de una órbita */
    int bxNumId;

    /* Parámetros de celda */
    float xCoorCell[4];         // Posicion horizontal del hit en la cámara
    float xDriftDistance[4];    // Distancia de deriva en la celda (sin signo)

    float tanPhi;
    float horizPos;

    float chiSquare;
};

#endif
