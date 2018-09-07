#include "dtsegmentinfo.h"
#include <cstring>  // Para función "memcpy"
#include "logmacros.h"

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
DTSegmentInfo::DTSegmentInfo() {
  LOGTRACE("Creando un 'DTSegmentInfo'");

  bxTimeValue = -1;
  bxNumId     = -1;
  tanPhi      = 0;
  horizPos    = 0;
  chiSquare   = 0;
  for (int i = 0; i <= 3; i++) {
    lateralComb[i] = LEFT;
    setXCoorCell     ( 0, i );
    setDriftDistance ( 0, i );
  }
}

DTSegmentInfo::DTSegmentInfo(DTSegmentInfo *ptr) {
  LOGTRACE("Clonando un 'DTSegmentInfo'");

  setLateralComb ( ptr->getLateralComb() );
  setBxTimeValue ( ptr->getBxTimeValue() );
  setTanPhi      ( ptr->getTanPhi()      );
  setHorizPos    ( ptr->getHorizPos()    );
  setChiSq       ( ptr->getChiSq()       );

  for (int i = 0; i <= 3; i++) {
    setXCoorCell     ( ptr->getXCoorCell(i), i     );
    setDriftDistance ( ptr->getDriftDistance(i), i );
  }
}

DTSegmentInfo::~DTSegmentInfo() {
  LOGTRACE("Destruyendo un 'DTSegmentInfo'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
/**
 * Informa si este contenedor tiene datos válidos. Se considera inválido si
 * el segmento tiene un tiempo negativo (no se ha asignado nunca un valor de
 * tiempo de BX).
 */
bool DTSegmentInfo::isValid(void) { return (bxTimeValue != -1); }

void DTSegmentInfo::setBxTimeValue(int time) {
  bxTimeValue = time;

  float auxBxId = float(time) / LHC_CLK_FREQ;
  bxNumId = int(auxBxId);
  if ( (auxBxId - int(auxBxId)) >= 0.5 ) bxNumId = int(bxNumId + 1);
}

int DTSegmentInfo::getBxTimeValue(void) { return bxTimeValue; }
int DTSegmentInfo::getBxNumId(void) { return bxNumId; }

/* Este método será invocado por el analizador para rellenar la información
   sobre la combinación de lateralidad que ha dado lugar a una trayectoria
   válida. Antes de ese momento, no tiene utilidad alguna */
void DTSegmentInfo::setLateralComb(LATERAL_CASES latComb[4]) {
  memcpy(lateralComb, latComb, 4 * sizeof(LATERAL_CASES));
}

void DTSegmentInfo::setLateralComb(const LATERAL_CASES *latComb) {
  memcpy(lateralComb, latComb, 4 * sizeof(LATERAL_CASES));
}

const LATERAL_CASES* DTSegmentInfo::getLateralComb(void) { 
  return (lateralComb); 
}

void  DTSegmentInfo::setHorizPos(float pos) { horizPos = pos; }
float DTSegmentInfo::getHorizPos(void) { return horizPos; }

void  DTSegmentInfo::setTanPhi(float tanPhi) { this->tanPhi = tanPhi; }
float DTSegmentInfo::getTanPhi(void) { return tanPhi; }

void  DTSegmentInfo::setChiSq(float chi) { chiSquare = chi;  }
float DTSegmentInfo::getChiSq(void)      { return chiSquare; }

void  DTSegmentInfo::setXCoorCell(float x, int cell) { xCoorCell[cell] = x;    }
float DTSegmentInfo::getXCoorCell(int cell)          { return xCoorCell[cell]; }

void  DTSegmentInfo::setDriftDistance(float dx, int cell) {
  xDriftDistance[cell] = dx;
}
float DTSegmentInfo::getDriftDistance(int cell) { return xDriftDistance[cell]; }
