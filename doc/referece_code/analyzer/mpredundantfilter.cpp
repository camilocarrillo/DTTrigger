/**
 * Project:
 * Subproject: tracrecons
 * File name:  mpredundantfilter.cpp
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
#include "mpredundantfilter.h"
#include "logmacros.h"
#include "muonpath.h"

namespace CMS {
  
//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
MPRedundantFilter::MPRedundantFilter(I_Popper &dataIn, I_Pusher &dataOut,
                                     int bufSize) :
  I_Filter(dataIn, dataOut), 
  buffer(bufSize)
{
  LOGTRACE("Creando un 'MPRedundantFilter'");
  // Eliminaremos el primer elemento del anillo cuando introduzcamos uno nuevo.
  buffer.deleteObjsOnForget(true);
}

MPRedundantFilter::~MPRedundantFilter()
{
  LOGTRACE("Destruyendo un 'MPRedundantFilter'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void MPRedundantFilter::filter(void) {

  MuonPath *mPath = (MuonPath*) dataIn.pop();
  /*
    Esta línea se incluye para evitar que, tras un 'stop', que fuerza la
    liberación del mutex de la fifo de entrada, devuelva un puntero nulo, lo que
    a su vez, induce un error en la ejecución al intentar acceder a cualquiera
    de los métodos de la clase 'DTPrimitive'
   */
  if (mPath == NULL) return;
  
  // En caso que no esté en el buffer, será enviado al exterior.
  if ( !isInBuffer(mPath) ) {
    // Borramos el primer elemento que se insertó (el más antiguo).
    if (buffer.size() <= buffer.storedElements()) buffer.forgetData(1);
    // Insertamos el ultimo "path" como nuevo elemento.
    buffer.push(mPath);
    
    // Enviamos una copia
    MuonPath *mpAux = new MuonPath(mPath);
    dataOut.push( mpAux );
  }
}

void MPRedundantFilter::reset(void) { buffer.reset(); }

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
/**
 * Devuelve TRUE si el "mPath" está ya en el buffer de comparacion.
 */
bool MPRedundantFilter::isInBuffer(MuonPath* mPath) {
  bool ans = false;
  
  if ( !buffer.isEmpty() )
    for (int i = 0; i < buffer.storedElements(); i++) 
      /*
       * Recorremos el buffer is si detectamos un elemento igual al de prueba
       * salimos, indicando el resultado.
       * No se siguen procesando los restantes elementos.
       */
      if ( mPath->isEqualTo( (MuonPath*) buffer.readAt(i) )) {
        ans = true;
        break;
      }
    
  return ans;
}

}
