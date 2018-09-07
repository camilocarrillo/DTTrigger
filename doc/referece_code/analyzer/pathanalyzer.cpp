/**
 * Project:    cms
 * Subproject: tracrecons
 * File name:  pathanalyzer.cpp
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
 * Copyright (c) 2015 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#include "pathanalyzer.h"
#include "dtprimitive.h"
#include "logmacros.h"
#include "cexception.h"

#include <cstring>  // Para función "memcpy"
#include <cstdlib>  // Para función "abs"
#include <cmath>

namespace CMS {

/* Para cada combinación de 4 celdas (una por capa) que forman un MuonPath,
este array almacena cada una de las combinaciones de 3 capas sobre las que
se aplicará el "mean-timer" */
const int PathAnalyzer::LAYER_ARRANGEMENTS[MAX_VERT_ARRANG][3] = {
  {0, 1, 2}, {1, 2, 3},                       // Grupos consecutivos
  {0, 1, 3}, {0, 2, 3}                        // Grupos salteados
};

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
PathAnalyzer::PathAnalyzer(I_Popper &inMuonPath, I_Pusher &outValidMuonPath) :
  I_Task(),
  inMuonPath(inMuonPath),
  outValidMuonPath(outValidMuonPath)
{
  LOGTRACE("Creando un 'PathAnalyzer'");
  bxTolerance = 0;
  minQuality  = LOWQGHOST;
  setRRobin(false);
  reset();
  
  chiSquareThreshold = 50;
}

PathAnalyzer::~PathAnalyzer() {
  LOGTRACE("Destruyendo un 'PathAnalyzer'");
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void PathAnalyzer::run(void)
{
  LOGTRACEFN_IN;

  while(roundRobin)
    analyze();

  LOGTRACEFN_OUT;
}

void PathAnalyzer::stop(void) { setRRobin(false); }
void PathAnalyzer::setRRobin(bool rr) { roundRobin = rr; }

void PathAnalyzer::analyze(void) {

  MuonPath *mPath = NULL;

  if ( (mPath = (MuonPath*) inMuonPath.pop()) != NULL ) {
    if (mPath->isAnalyzable()) {
      LOGINFO("---");
      LOGINFO("*************************************");
      LOGINFO(
        NString("Analizando MuonPath. Capa/canal: ").\
          nAppend("%i", mPath->getPrimitive(0)->getLayerId()).sAppend("/").\
          nAppend("%i", mPath->getPrimitive(0)->getChannelId()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(1)->getLayerId()).sAppend("/").\
          nAppend("%i", mPath->getPrimitive(1)->getChannelId()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(2)->getLayerId()).sAppend("/").\
          nAppend("%i", mPath->getPrimitive(2)->getChannelId()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(3)->getLayerId()).sAppend("/").\
          nAppend("%i", mPath->getPrimitive(3)->getChannelId()).sAppend(" "));

      LOGINFO(
        NString("Candidate. TDC Time's: ").\
          nAppend("%i", mPath->getPrimitive(0)->getTDCTime()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(1)->getTDCTime()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(2)->getTDCTime()).sAppend(" ").\
          nAppend("%i", mPath->getPrimitive(3)->getTDCTime()).sAppend(" "));


      setCellLayout( mPath->getCellHorizontalLayout() );
      evaluatePathQuality(mPath);
      stats.mpAnalized++;
    }
    else {
      delete mPath;
      stats.mpNotAnalized++;
    }
  }
  else stats.mpNull++;

  /* Después del análisis, si la trayectoria no es válida, destruimos el
     objeto (que debería destruir, a su vez, las primitivas que lo componen)
     o, en caso contrario, lo enviamos a la fifo de salida de MuonPath's
     válidos */
  if ( mPath != NULL ) {
    if ( mPath->getQuality() >= minQuality ) {
        LOGINFO(NString("MuonPath Valido. Calidad: ").\
                nAppend("%i", mPath->getQuality()));

        for (int i = 0; i <= 3; i++)
          LOGINFO(
            NString("Capa: ").\
                   nAppend("%i", mPath->getPrimitive(i)->getLayerId()).\
                   sAppend(" Canal: ").\
                   nAppend("%i", mPath->getPrimitive(i)->getChannelId()).\
                   sAppend(" TDCTime: ").
                   nAppend("%i", mPath->getPrimitive(i)->getTDCTime()));
      /*
       * Clonamos el objeto tantas veces como lateralidades válidas haya 
       * que tengan el mismo nivel de calidad que el que "evaluatePathQuality"
       * haya estimado.
       * Esta precaución es así porque puede haber salido para una combinación
       * de hits dada, por ejemplo una lateralidad de alta calidad y varias 
       * otras de baja. Estas últimas no han de ser enviadas. Así que la 
       * comparación del nivel de la calidad almacenada en el array 
       * "lateralities" con la del "mPath" es obligada.
       */
      for (int i = 0; i < totalNumValLateralities; i++) {
        /*
         * Puede ocurrir que un segmento contenga, p. ej., una lat-comb 
         * HIGHQ y varias adicionales LOWQ, en cuyo caso sólo hay que enviar
         * aquellas lat-comb cuya calidad sea análoga a la asignada como
         * global. Es decir: si se asigna global HIGHQ o HIGHQGHOST, se envían
         * aquella de calidad parcial HIGHQ; si LOWQ o LOWQGHOST, se envían 
         * las de calidad parcial LOWQ.
         */
        if (latQuality[i].valid && 
            (
              ((mPath->getQuality() == HIGHQ || 
                mPath->getQuality() == HIGHQGHOST) && 
                latQuality[i].quality == HIGHQ)             ||
              ((mPath->getQuality() == LOWQ  ||
                mPath->getQuality() == LOWQGHOST)  && 
                latQuality[i].quality == LOWQ)
            )
        )
        {
          // Clonamos el objeto analizado.
          MuonPath *mpAux = new MuonPath(mPath);
          mpAux->setBxTimeValue(latQuality[i].bxValue);
          mpAux->setLateralComb(lateralities[i]);
          /* 
           * Si hay que invalidar algún 'hit en alguna combinación de
           * lateralidad concreta, lo eliminamos del 'segmento'.
           * 
           * De forma chapucera lo hacemos eliminando la primitiva, y 
           * reemplazándola por una nueva vacía (se crean inválidas).
           */
          int idxHitNotValid = latQuality[i].invalidateHitIdx;
          if (idxHitNotValid >= 0) {
            delete mpAux->getPrimitive(idxHitNotValid);
            mpAux->setPrimitive(new DTPrimitive(), idxHitNotValid);
          }

          calculatePathParameters(mpAux);
          /* 
           * Si, tras calcular los parámetros, y si se trata de un segmento
           * con 4 hits, el chi2 resultante es superior al umbral programado,
           * lo eliminamos y no se envía al exterior.
           * Y pasamos al siguiente elemento.
           */
          if ((mpAux->getQuality() == HIGHQ || 
               mpAux->getQuality() == HIGHQGHOST) && 
               mpAux->getChiSq() > chiSquareThreshold) {
            delete mpAux;
          }
          else {
            LOGINFO_INT("BX Time = ", mpAux->getBxTimeValue());
            LOGINFO_INT("BX Id   = ", mpAux->getBxNumId());
            LOGINFO_F("XCoor   = ", mpAux->getHorizPos());
            LOGINFO_F("tan(Phi)= ", mpAux->getTanPhi());

            outValidMuonPath.push(mpAux);
            stats.mpWithValidPath++;
          }
        }
      }
      /*
       * En esta nueva versión enviamos copias, así que borramos
       * el objeto original.
       */
      delete mPath;
    }
    else {
      delete mPath;
      stats.mpNoValidPath++;
    }
  }
}

void PathAnalyzer::reset(void) {
  stats.mpAnalized      = 0;
  stats.mpNull          = 0;
  stats.mpNotAnalized   = 0;
  stats.mpWithValidPath = 0;
  stats.mpNoValidPath   = 0;
  stats.mpHighQ         = 0;
  stats.mpHighQGhost    = 0;
  stats.mpLowQ          = 0;
  stats.mpLowQGhost     = 0;
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void PathAnalyzer::setBXTolerance(int t) { bxTolerance = t; }
int  PathAnalyzer::getBXTolerance(void)  { return bxTolerance; }

void PathAnalyzer::setChiSquareThreshold(float ch2Thr) {
  chiSquareThreshold = ch2Thr;
}

void PathAnalyzer::setMinimumQuality(MP_QUALITY q) {
  if (minQuality >= LOWQGHOST) minQuality = q;
}
MP_QUALITY PathAnalyzer::getMinimumQuality(void) { return minQuality; }

const PathAnalyzer::STATISTICS* PathAnalyzer::getStatistics(void) {
  return &stats;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void PathAnalyzer::setCellLayout(const int layout[4]) {
  memcpy(cellLayout, layout, 4 * sizeof(int));
  /*
    Siempre que se cambie la combinación de celdas hay que volver a evaluar
    las combinaciones válidas de lateralidades
   */
  buildLateralities();
}

/**
 * Para una combinación de 4 celdas dada (las que se incluyen en el analizador,
 * una por capa), construye de forma automática todas las posibles
 * combinaciones de lateralidad (LLLL, LLRL,...) que son compatibles con una
 * trayectoria recta. Es decir, la partícula no hace un zig-zag entre los hilos
 * de diferentes celdas, al pasar de una a otra.
 */
void PathAnalyzer::buildLateralities(void) {

  LOGTRACEFN_IN;
  LATERAL_CASES (*validCase)[4], sideComb[4];

  totalNumValLateralities = 0;
  /* Generamos todas las posibles combinaciones de lateralidad para el grupo
     de celdas que forman parte del analizador */
  for(int lowLay = LEFT; lowLay <= RIGHT; lowLay++)
    for(int midLowLay = LEFT; midLowLay <= RIGHT; midLowLay++)
      for(int midHigLay = LEFT; midHigLay <= RIGHT; midHigLay++)
        for(int higLay = LEFT; higLay <= RIGHT; higLay++) {

          sideComb[0] = static_cast<LATERAL_CASES>(lowLay);
          sideComb[1] = static_cast<LATERAL_CASES>(midLowLay);
          sideComb[2] = static_cast<LATERAL_CASES>(midHigLay);
          sideComb[3] = static_cast<LATERAL_CASES>(higLay);

          /* Si una combinación de lateralidades es válida, la almacenamos */
          if (isStraightPath(sideComb)) {
            validCase = lateralities + totalNumValLateralities;
            memcpy(validCase, sideComb, 4 * sizeof(LATERAL_CASES));

            latQuality[totalNumValLateralities].valid            = false;
            latQuality[totalNumValLateralities].bxValue          = 0;
            latQuality[totalNumValLateralities].quality          = NOPATH;
            latQuality[totalNumValLateralities].invalidateHitIdx = -1;

            totalNumValLateralities++;
          }
        }
}

/**
 * Para automatizar la generación de trayectorias compatibles con las posibles
 * combinaciones de lateralidad, este método decide si una cierta combinación
 * de lateralidad, involucrando 4 celdas de las que conforman el PathAnalyzer,
 * forma una traza recta o no. En caso negativo, la combinación de lateralidad
 * es descartada y no se analiza.
 * En el caso de implementación en FPGA, puesto que el diseño intentará
 * paralelizar al máximo la lógica combinacional, el equivalente a este método
 * debería ser un "generate" que expanda las posibles combinaciones de
 * lateralidad de celdas compatibles con el análisis.
 *
 * El métoda da por válida una trayectoria (es recta) si algo parecido al
 * cambio en la pendiente de la trayectoria, al cambiar de par de celdas
 * consecutivas, no es mayor de 1 en unidades arbitrarias de semi-longitudes
 * de celda para la dimensión horizontal, y alturas de celda para la vertical.
 */
bool PathAnalyzer::isStraightPath(LATERAL_CASES sideComb[4]) {

  LOGTRACEFN_IN;
  int i, ajustedLayout[4], pairDiff[3], desfase[3];

  /* Sumamos el valor de lateralidad (LEFT = 0, RIGHT = 1) al desfase
     horizontal (respecto de la celda base) para cada celda en cuestion */
  for(i = 0; i <= 3; i++) ajustedLayout[i] = cellLayout[i] + sideComb[i];
  /* Variación del desfase por pares de celdas consecutivas */
  for(i = 0; i <= 2; i++) pairDiff[i] = ajustedLayout[i+1] - ajustedLayout[i];
  /* Variación de los desfases entre todas las combinaciones de pares */
  for(i = 0; i <= 1; i++) desfase[i] = abs(pairDiff[i+1] - pairDiff[i]);
  desfase[2] = abs(pairDiff[2] - pairDiff[0]);
  /* Si algún desfase es mayor de 2 entonces la trayectoria no es recta */
  bool resultado = (desfase[0] > 1 || desfase[1] > 1 || desfase[2] > 1);

  return ( !resultado );
}

/**
 * Recorre las calidades calculadas para todas las combinaciones de lateralidad
 * válidas, para determinar la calidad final asignada al "MuonPath" con el que
 * se está trabajando.
 */
void PathAnalyzer::evaluatePathQuality(MuonPath *mPath) {

  LOGTRACEFN_IN;
  int totalHighQ = 0, totalLowQ = 0;

  LOGDEBUG_UINT("Evaluando PathQ. Celda base: ", mPath->getBaseChannelId());
  LOGDEBUG_UINT("Total lateralidades: ", totalNumValLateralities);

  // Por defecto.
  mPath->setQuality(NOPATH);

  /* Ensayamos los diferentes grupos de lateralidad válidos que constituyen
     las posibles trayectorias del muón por el grupo de 4 celdas.
     Posiblemente esto se tenga que optimizar de manera que, si en cuanto se
     encuentre una traza 'HIGHQ' ya no se continue evaluando mas combinaciones
     de lateralidad, pero hay que tener en cuenta los fantasmas (rectas
     paralelas) en de alta calidad que se pueden dar en los extremos del BTI.
     Posiblemente en la FPGA, si esto se paraleliza, no sea necesaria tal
     optimización */
  for (int latIdx = 0; latIdx < totalNumValLateralities; latIdx++) {
    LOGINFO(
      NString("\n**********\n**** Analizando combinacion de lateralidad: ").\
        nAppend("%i", lateralities[latIdx][0]).sAppend("/").\
        nAppend("%i", lateralities[latIdx][1]).sAppend("/").\
        nAppend("%i", lateralities[latIdx][2]).sAppend("/").\
        nAppend("%i", lateralities[latIdx][3]).sAppend(".").\
        sAppend("\n**********"));

    evaluateLateralQuality(latIdx, mPath, &(latQuality[latIdx]));

    if (latQuality[latIdx].quality == HIGHQ) {
      totalHighQ++;
      LOGINFO("Lateralidad HIGHQ");
    }
    if (latQuality[latIdx].quality == LOWQ) {
      totalLowQ++;
      LOGINFO("Lateralidad LOWQ");
    }
  }
  /*
   * Establecimiento de la calidad.
   */
  if (totalHighQ == 1) {
    mPath->setQuality(HIGHQ);
    stats.mpHighQ++;
  }
  else if (totalHighQ > 1) {
    mPath->setQuality(HIGHQGHOST);
    stats.mpHighQGhost++;
  }
  else if (totalLowQ == 1) {
    mPath->setQuality(LOWQ);
    stats.mpLowQ++;
  }
  else if (totalLowQ > 1) {
    mPath->setQuality(LOWQGHOST);
    stats.mpLowQGhost++;
  }
}

void PathAnalyzer::evaluateLateralQuality(int latIdx, MuonPath *mPath,
                                          LATQ_TYPE *latQuality)
{
  int layerGroup[3];
  LATERAL_CASES sideComb[3];
  PARTIAL_LATQ_TYPE latQResult[4] = {
    {false, 0}, {false, 0}, {false, 0}, {false, 0}
  };

  // Default values.
  latQuality->valid            = false;
  latQuality->bxValue          = 0;
  latQuality->quality          = NOPATH;
  latQuality->invalidateHitIdx = -1;

  /* En el caso que, para una combinación de lateralidad dada, las 2
     combinaciones consecutivas de 3 capas ({0, 1, 2}, {1, 2, 3}) fueran
     traza válida, habríamos encontrado una traza correcta de alta calidad,
     por lo que sería innecesario comprobar las otras 2 combinaciones
     restantes.
     Ahora bien, para reproducir el comportamiento paralelo de la FPGA en el
     que el análisis se va a evaluar simultáneamente en todas ellas,
     construimos un código que analiza las 4 combinaciones, junto con una
     lógica adicional para discriminar la calidad final de la traza */
  for (int i = 0; i <= 3 ; i++) {
    memcpy(layerGroup, LAYER_ARRANGEMENTS[i], 3 * sizeof(int));

    // Seleccionamos la combinación de lateralidad para cada celda.
    for (int j = 0; j < 3; j++)
      sideComb[j] = lateralities[latIdx][ layerGroup[j] ];

    validate(sideComb, layerGroup, mPath, &(latQResult[i]));
  }
  /*
    Imponemos la condición, para una combinación de lateralidad completa, que
    todas las lateralidades parciales válidas arrojen el mismo valor de BX
    (dentro de un margen) para así dar una traza consistente.
    En caso contrario esa combinación se descarta.
  */
  if ( !sameBXValue(latQResult) ) {
    // Se guardan en los default values inciales.
    LOGINFO("Lateralidad DESCARTADA. Tolerancia de BX excedida");
    return;
  }

  // Dos trazas complementarias válidas => Traza de muón completa.
  if ((latQResult[0].latQValid && latQResult[1].latQValid) ||
      (latQResult[0].latQValid && latQResult[2].latQValid) ||
      (latQResult[0].latQValid && latQResult[3].latQValid) ||
      (latQResult[1].latQValid && latQResult[2].latQValid) ||
      (latQResult[1].latQValid && latQResult[3].latQValid) ||
      (latQResult[2].latQValid && latQResult[3].latQValid))
  {
    latQuality->valid   = true;
//     latQuality->bxValue = latQResult[0].bxValue;
    /*
     * Se hace necesario el contador de casos "numValid", en vez de promediar
     * los 4 valores dividiendo entre 4, puesto que los casos de combinaciones
     * de 4 hits buenos que se ajusten a una combinación como por ejemplo:
     * L/R/L/L, dan lugar a que en los subsegmentos 0, y 1 (consecutivos) se
     * pueda aplicar mean-timer, mientras que en el segmento 3 (en el ejemplo
     * capas: 0,2,3, y combinación L/L/L) no se podría aplicar, dando un
     * valor parcial de BX = 0.
     */
    int sumBX = 0, numValid = 0;
    for (int i = 0; i <= 3; i++) {
      if (latQResult[i].latQValid) {
        sumBX += latQResult[i].bxValue;
        numValid++;
      }
    }

    latQuality->bxValue = sumBX / numValid;
    latQuality->quality = HIGHQ;

    LOGINFO("Lateralidad ACEPTADA. HIGHQ.");
  }
  // Sólo una traza disjunta válida => Traza de muón incompleta pero válida.
  else { 
    if (latQResult[0].latQValid || latQResult[1].latQValid ||
        latQResult[2].latQValid || latQResult[3].latQValid)
    {
      latQuality->valid   = true;
      latQuality->quality = LOWQ;
      for (int i = 0; i < 4; i++)
        if (latQResult[i].latQValid) {
          latQuality->bxValue = latQResult[i].bxValue;
          /*
           * En los casos que haya una combinación de 4 hits válidos pero
           * sólo 3 de ellos formen traza (calidad 2), esto permite detectar
           * la layer con el hit que no encaja en la recta, y así poder
           * invalidarlo, cambiando su valor por "-1" como si de una mezcla
           * de 3 hits pura se tratara.
           * Esto es útil para los filtros posteriores.
           */
          latQuality->invalidateHitIdx = getOmittedHit( i );
          break;
        }

      LOGINFO("Lateralidad ACEPTADA. LOWQ.");
    }
    else {
      LOGINFO("Lateralidad DESCARTADA. NOPATH.");
    }
  }
}

/**
 * Valida, para una combinación de capas (3), celdas y lateralidad, si los
 * valores temporales cumplen el criterio de mean-timer.
 * En vez de comparar con un 0 estricto, que es el resultado aritmético de las
 * ecuaciones usadas de base, se incluye en la clase un valor de tolerancia
 * que por defecto vale cero, pero que se puede ajustar a un valor más
 * adecuado
 *
 * En esta primera versión de la clase, el código de generación de ecuaciones
 * se incluye en esta función, lo que es ineficiente porque obliga a calcular
 * un montón de constantes, fijas para cada combinación de celdas, que
 * tendrían que evaluarse una sóla vez en el constructor de la clase.
 * Esta disposición en el constructor estaría más proxima a la realización que
 * se tiene que llevar a término en la FPGA (en tiempo de síntesis).
 * De momento se deja aquí porque así se entiende la lógica mejor, al estar
 * descrita de manera lineal en un sólo método.
 */
void PathAnalyzer::validate(LATERAL_CASES sideComb[3], int layerIndex[3],
                            MuonPath* mPath, PARTIAL_LATQ_TYPE *latq)
{
  // Valor por defecto.
  latq->bxValue   = 0;
  latq->latQValid = false;

  LOGDEBUG(
    NString("Iniciando validacion de MuonPath para capas: ").\
      nAppend("%i", layerIndex[0]).sAppend("/").\
      nAppend("%i", layerIndex[1]).sAppend("/").\
      nAppend("%i", layerIndex[2]));

  LOGDEBUG(
    NString("Lateralidades parciales: ").\
      nAppend("%i", sideComb[0]).sAppend("/").\
      nAppend("%i", sideComb[1]).sAppend("/").\
      nAppend("%i", sideComb[2]).sAppend("."));

  /* Primero evaluamos si, para la combinación concreta de celdas en curso, el
     número de celdas con dato válido es 3. Si no es así, sobre esa
     combinación no se puede aplicar el mean-timer y devolvemos "false" */
  int validCells = 0;
  for (int j = 0; j < 3; j++)
    if (mPath->getPrimitive(layerIndex[j])->isValidTime()) validCells++;

  if (validCells != 3) {
    LOGDEBUG("No hay 3 celdas validas.");
    return;
  }

  LOGDEBUG(
    NString("Valores de TDC: ").\
      nAppend("%i", mPath->getPrimitive(layerIndex[0])->getTDCTime()).sAppend("/").\
      nAppend("%i", mPath->getPrimitive(layerIndex[1])->getTDCTime()).sAppend("/").\
      nAppend("%i", mPath->getPrimitive(layerIndex[2])->getTDCTime()).sAppend("."));

  LOGDEBUG(
    NString("Valid TIMES: ").\
      nAppend("%i", mPath->getPrimitive(layerIndex[0])->isValidTime()).sAppend("/").\
      nAppend("%i", mPath->getPrimitive(layerIndex[1])->isValidTime()).sAppend("/").\
      nAppend("%i", mPath->getPrimitive(layerIndex[2])->isValidTime()).sAppend("."));

  /* Distancias verticales entre capas inferior/media y media/superior */
  int dVertMI = layerIndex[1] - layerIndex[0];
  int dVertSM = layerIndex[2] - layerIndex[1];

  /* Distancias horizontales entre capas inferior/media y media/superior */
  int dHorzMI = cellLayout[layerIndex[1]] - cellLayout[layerIndex[0]];
  int dHorzSM = cellLayout[layerIndex[2]] - cellLayout[layerIndex[1]];

  /* Índices de pares de capas sobre las que se está actuando
     SM => Superior + Intermedia
     MI => Intermedia + Inferior
     Jugamos con los punteros para simplificar el código */
  int *layPairSM = &layerIndex[1];
  int *layPairMI = &layerIndex[0];

  /* Pares de combinaciones de celdas para composición de ecuación. Sigue la
     misma nomenclatura que el caso anterior */
  LATERAL_CASES smSides[2], miSides[2];

  /* Teniendo en cuenta que en el índice 0 de "sideComb" se almacena la
     lateralidad de la celda inferior, jugando con aritmética de punteros
     extraemos las combinaciones de lateralidad para los pares SM y MI */
  memcpy(smSides, &sideComb[1], 2 * sizeof(LATERAL_CASES));
  memcpy(miSides, &sideComb[0], 2 * sizeof(LATERAL_CASES));

  float bxValue = 0;
  int coefsAB[2] = {0, 0}, coefsCD[2] = {0, 0};
  /* It's neccesary to be careful with that pointer's indirection. We need to
  retrieve the lateral coeficientes (+-1) from the lower/middle and
  middle/upper cell's lateral combinations. They are needed to evaluate the
  existance of a possible BX value, following it's calculation equation */
  getLateralCoeficients(miSides, coefsAB);
  getLateralCoeficients(smSides, coefsCD);

  /* Cada para de sumas de los 'coefsCD' y 'coefsAB' dan siempre como resultado
  0, +-2.

  A su vez, y pese a que las ecuaciones se han construido de forma genérica
  para cualquier combinación de celdas de la cámara, los valores de 'dVertMI' y
  'dVertSM' toman valores 1 o 2 puesto que los pares de celdas con los que se
  opera en realidad, o bien están contiguos, o bien sólo están separadas por
  una fila de celdas intermedia. Esto es debido a cómo se han combinado los
  grupos de celdas, para aplicar el mean-timer, en 'LAYER_ARRANGEMENTS'.

  El resultado final es que 'denominator' es siempre un valor o nulo, o
  múltiplo de 2 */
  int denominator = dVertMI * (coefsCD[1] + coefsCD[0]) -
                    dVertSM * (coefsAB[1] + coefsAB[0]);

  if(denominator == 0) {
    LOGDEBUG("Imposible calcular BX. Denominador para BX = 0.");
    return;
  }

  /* Esta ecuación ha de ser optimizada, especialmente en su implementación
  en FPGA. El 'denominator' toma siempre valores múltiplo de 2 o nulo, por lo
  habría que evitar el cociente y reemplazarlo por desplazamientos de bits */
  bxValue = (
    dVertMI*(dHorzSM*MAXDRIFT + eqMainBXTerm(smSides, layPairSM, mPath)) -
    dVertSM*(dHorzMI*MAXDRIFT + eqMainBXTerm(miSides, layPairMI, mPath))
            ) / denominator;

  if(bxValue < 0) {
    LOGDEBUG("Combinacion no valida. BX Negativo.");
    return;
  }

  // Redondeo del valor del tiempo de BX al nanosegundo
  if ( (bxValue - int(bxValue)) >= 0.5 ) bxValue = float(int(bxValue + 1));
  else bxValue = float(int(bxValue));

  /* Ciertos valores del tiempo de BX, siendo positivos pero objetivamente no
  válidos, pueden dar lugar a que el discriminador de traza asociado de un
  valor aparentemente válido (menor que la tolerancia y típicamente 0). Eso es
  debido a que el valor de tiempo de BX es mayor que algunos de los tiempos
  de TDC almacenados en alguna de las respectivas 'DTPrimitives', lo que da
  lugar a que, cuando se establece el valore de BX para el 'MuonPath', se
  obtengan valores de tiempo de deriva (*NO* tiempo de TDC) en la 'DTPrimitive'
  nulos, o inconsistentes, a causa de la resta entre enteros.

  Así pues, se impone como criterio de validez adicional que el valor de tiempo
  de BX (bxValue) sea siempre superior a cualesquiera valores de tiempo de TDC
  almacenados en las 'DTPrimitives' que forman el 'MuonPath' que se está
  analizando.
  En caso contrario, se descarta como inválido */

  for (int i = 0; i < 3; i++)
    if (mPath->getPrimitive(layerIndex[i])->isValidTime()) {
      int diffTime =
          mPath->getPrimitive(layerIndex[i])->getTDCTimeNoOffset() - bxValue;

      if (diffTime < 0 || diffTime > MAXDRIFT) {
        LOGDEBUG("Valor de BX inválido. Al menos un tiempo de TDC sin sentido");
        return;
      }
    }

  /* Si se llega a este punto, el valor de BX y la lateralidad parcial se dan
   * por válidas.
   */
  latq->bxValue   = bxValue;
  latq->latQValid = true;
}

/**
 * Evalúa la suma característica de cada par de celdas, según la lateralidad
 * de la trayectoria.
 * El orden de los índices de capa es crítico:
 *    layerIdx[0] -> Capa más baja,
 *    layerIdx[1] -> Capa más alta
 */
int PathAnalyzer::eqMainBXTerm(LATERAL_CASES sideComb[2], int layerIdx[2],
                               MuonPath* mPath)
{
  int eqTerm = 0, coefs[2];

  getLateralCoeficients(sideComb, coefs);

  eqTerm = coefs[0] * mPath->getPrimitive(layerIdx[0])->getTDCTimeNoOffset() +
           coefs[1] * mPath->getPrimitive(layerIdx[1])->getTDCTimeNoOffset();

  LOGDEBUG_INT("EQTerm(BX): ", eqTerm);

  return (eqTerm);
}

/**
 * Evalúa la suma característica de cada par de celdas, según la lateralidad
 * de la trayectoria. Semejante a la anterior, pero aplica las correcciones
 * debidas a los retardos de la electrónica, junto con la del Bunch Crossing
 *
 * El orden de los índices de capa es crítico:
 *    layerIdx[0] -> Capa más baja,
 *    layerIdx[1] -> Capa más alta
 */
int PathAnalyzer::eqMainTerm(LATERAL_CASES sideComb[2], int layerIdx[2],
                             MuonPath* mPath, int bxValue)
{
  int eqTerm = 0, coefs[2];

  getLateralCoeficients(sideComb, coefs);

  eqTerm = coefs[0] * (mPath->getPrimitive(layerIdx[0])->getTDCTimeNoOffset() -
                       bxValue) +
           coefs[1] * (mPath->getPrimitive(layerIdx[1])->getTDCTimeNoOffset() -
                       bxValue);

  LOGDEBUG_INT("EQTerm(Main): ", eqTerm);

  return (eqTerm);
}

/**
 * Devuelve los coeficientes (+1 ó -1) de lateralidad para un par dado.
 * De momento es útil para poder codificar la nueva funcionalidad en la que se
 * calcula el BX.
 */
void PathAnalyzer::getLateralCoeficients(LATERAL_CASES sideComb[2],
                                         int *coefs)
{
  if ((sideComb[0] == LEFT) && (sideComb[1] == LEFT)) {
    *(coefs)     = +1;
    *(coefs + 1) = -1;
  }
  else if ((sideComb[0] == LEFT) && (sideComb[1] == RIGHT)){
    *(coefs)     = +1;
    *(coefs + 1) = +1;
  }
  else if ((sideComb[0] == RIGHT) && (sideComb[1] == LEFT)){
    *(coefs)     = -1;
    *(coefs + 1) = -1;
  }
  else if ((sideComb[0] == RIGHT) && (sideComb[1] == RIGHT)){
    *(coefs)     = -1;
    *(coefs + 1) = +1;
  }
}

/**
 * Determines if all valid partial lateral combinations share the same value
 * of 'bxValue'.
 */
bool PathAnalyzer::sameBXValue(PARTIAL_LATQ_TYPE* latq) {

  bool result = true;
  /*
  Para evitar los errores de precision en el cálculo, en vez de forzar un
  "igual" estricto a la hora de comparar los diferentes valores de BX, se
  obliga a que la diferencia entre pares sea menor que un cierto valor umbral.
  Para hacerlo cómodo se crean 6 booleanos que evalúan cada posible diferencia
  */
  bool d01, d02, d03, d12, d13, d23;
  d01 = (abs(latq[0].bxValue - latq[1].bxValue) <= bxTolerance) ? true : false;
  d02 = (abs(latq[0].bxValue - latq[2].bxValue) <= bxTolerance) ? true : false;
  d03 = (abs(latq[0].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;
  d12 = (abs(latq[1].bxValue - latq[2].bxValue) <= bxTolerance) ? true : false;
  d13 = (abs(latq[1].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;
  d23 = (abs(latq[2].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;

  /* Casos con 4 grupos de combinaciones parciales de lateralidad válidas */
  if ((latq[0].latQValid && latq[1].latQValid && latq[2].latQValid &&
       latq[3].latQValid) && !(d01 && d12 && d23))
    result = false;
  else
    /* Los 4 casos posibles de 3 grupos de lateralidades parciales válidas */
    if ( ((latq[0].latQValid && latq[1].latQValid && latq[2].latQValid) &&
          !(d01 && d12)
          )
        ||
         ((latq[0].latQValid && latq[1].latQValid && latq[3].latQValid) &&
          !(d01 && d13)
          )
        ||
         ((latq[0].latQValid && latq[2].latQValid && latq[3].latQValid) &&
          !(d02 && d23)
          )
        ||
         ((latq[1].latQValid && latq[2].latQValid && latq[3].latQValid) &&
          !(d12 && d23)
          )
        )
      result = false;
    else
      /* Por último, los 6 casos posibles de pares de lateralidades parciales
      válidas */
      if ( ((latq[0].latQValid && latq[1].latQValid) && !d01) ||
           ((latq[0].latQValid && latq[2].latQValid) && !d02) ||
           ((latq[0].latQValid && latq[3].latQValid) && !d03) ||
           ((latq[1].latQValid && latq[2].latQValid) && !d12) ||
           ((latq[1].latQValid && latq[3].latQValid) && !d13) ||
           ((latq[2].latQValid && latq[3].latQValid) && !d23) )
        result = false;

  return result;
}

/** Calcula los parámetros de la(s) trayectoria(s) detectadas.
 *
 * Asume que el origen de coordenadas está en al lado 'izquierdo' de la cámara
 * con el eje 'X' en la posición media vertical de todas las celdas.
 * El eje 'Y' se apoya sobre los hilos de las capas 1 y 3 y sobre los costados
 * de las capas 0 y 2.
 */
void PathAnalyzer::calculatePathParameters(MuonPath* mPath) {

  // El orden es importante. No cambiar sin revisar el código.
  calcCellDriftAndXcoor(mPath);

//   calcTanPhiXPosChamber(mPath);
  if (mPath->getQuality() == HIGHQ || mPath->getQuality() == HIGHQGHOST)
    calcTanPhiXPosChamber4Hits(mPath);
  else
    calcTanPhiXPosChamber3Hits(mPath);

  calcChiSquare(mPath);
}

void PathAnalyzer::calcTanPhiXPosChamber(MuonPath* mPath)
{
  /*
  La mayoría del código de este método tiene que ser optimizado puesto que
  se hacen llamadas y cálculos redundantes que ya se han evaluado en otros
  métodos previos.

  Hay que hacer una revisión de las ecuaciones para almacenar en el 'MuonPath'
  una serie de parámetro característicos (basados en sumas y productos, para
  que su implementación en FPGA sea sencilla) con los que, al final del
  proceso, se puedan calcular el ángulo y la coordenada horizontal.

  De momento se deja este código funcional extraído directamente de las
  ecuaciones de la recta.
  */
  int layerIdx[2];
  /*
  To calculate path's angle are only necessary two valid primitives.
  This method should be called only when a 'MuonPath' is determined as valid,
  so, at least, three of its primitives must have a valid time.
  With this two comparitions (which can be implemented easily as multiplexors
  in the FPGA) this method ensures to catch two of those valid primitives to
  evaluate the angle.

  The first one is below the middle line of the superlayer, while the other
  one is above this line
  */
  if (mPath->getPrimitive(0)->isValidTime()) layerIdx[0] = 0;
  else layerIdx[0] = 1;

  if (mPath->getPrimitive(3)->isValidTime()) layerIdx[1] = 3;
  else layerIdx[1] = 2;

  /* We identify along which cells' sides the muon travels */
  LATERAL_CASES sideComb[2];
  sideComb[0] = (mPath->getLateralComb())[ layerIdx[0] ];
  sideComb[1] = (mPath->getLateralComb())[ layerIdx[1] ];

  /* Horizontal gap between cells in cell's semi-length units */
  int dHoriz = (mPath->getCellHorizontalLayout())[ layerIdx[1] ] -
               (mPath->getCellHorizontalLayout())[ layerIdx[0] ];

  /* Vertical gap between cells in cell's height units */
  int dVert = layerIdx[1] -layerIdx[0];

  /*-----------------------------------------------------------------*/
  /*--------------------- Phi angle calculation ---------------------*/
  /*-----------------------------------------------------------------*/
  float num = CELL_SEMILENGTH * dHoriz +
              DRIFT_SPEED *
                 eqMainTerm(sideComb, layerIdx, mPath,
                            mPath->getBxTimeValue()
                           );

  float denom = CELL_HEIGHT * dVert;
  float tanPhi = num / denom;

  mPath->setTanPhi(tanPhi);

  /*-----------------------------------------------------------------*/
  /*----------------- Horizontal coord. calculation -----------------*/
  /*-----------------------------------------------------------------*/

  /*
  Using known coordinates, relative to superlayer axis reference, (left most
  superlayer side, and middle line between 2nd and 3rd layers), calculating
  horizontal coordinate implies using a basic line equation:
                      (y - y0) = (x - x0) * cotg(Phi)
  This horizontal coordinate can be obtained setting y = 0 on last equation,
  and also setting y0 and x0 with the values of a known muon's path cell
  position hit.
  It's enough to use the lower cell (layerIdx[0]) coordinates. So:
                      xC = x0 - y0 * tan(Phi)
  */
  float lowerXPHorizPos = mPath->getXCoorCell( layerIdx[0] );

  float lowerXPVertPos = 0; // This is only the absolute value distance.
  if (layerIdx[0] == 0) lowerXPVertPos = CELL_HEIGHT + CELL_SEMIHEIGHT;
  else                  lowerXPVertPos = CELL_SEMIHEIGHT;

  mPath->setHorizPos( lowerXPHorizPos + lowerXPVertPos * tanPhi );
}

/**
 * Cálculos de coordenada y ángulo para un caso de 4 HITS de alta calidad.
 */
void PathAnalyzer::calcTanPhiXPosChamber4Hits(MuonPath* mPath) {
  float tanPhi = (3 * mPath->getXCoorCell(3) +
                      mPath->getXCoorCell(2) -
                      mPath->getXCoorCell(1) -
                  3 * mPath->getXCoorCell(0)) / (10 * CELL_HEIGHT);

  mPath->setTanPhi(tanPhi);

  float XPos = (mPath->getXCoorCell(0) +
                mPath->getXCoorCell(1) +
                mPath->getXCoorCell(2) +
                mPath->getXCoorCell(3)) / 4;

  mPath->setHorizPos( XPos );
}

/**
 * Cálculos de coordenada y ángulo para un caso de 3 HITS.
 */
void PathAnalyzer::calcTanPhiXPosChamber3Hits(MuonPath* mPath) {
  int layerIdx[2];

  if (mPath->getPrimitive(0)->isValidTime()) layerIdx[0] = 0;
  else layerIdx[0] = 1;

  if (mPath->getPrimitive(3)->isValidTime()) layerIdx[1] = 3;
  else layerIdx[1] = 2;

  /* We identify along which cells' sides the muon travels */
  LATERAL_CASES sideComb[2];
  sideComb[0] = (mPath->getLateralComb())[ layerIdx[0] ];
  sideComb[1] = (mPath->getLateralComb())[ layerIdx[1] ];

  /* Horizontal gap between cells in cell's semi-length units */
  int dHoriz = (mPath->getCellHorizontalLayout())[ layerIdx[1] ] -
               (mPath->getCellHorizontalLayout())[ layerIdx[0] ];

  /* Vertical gap between cells in cell's height units */
  int dVert = layerIdx[1] -layerIdx[0];

  /*-----------------------------------------------------------------*/
  /*--------------------- Phi angle calculation ---------------------*/
  /*-----------------------------------------------------------------*/
  float num = CELL_SEMILENGTH * dHoriz +
              DRIFT_SPEED *
                 eqMainTerm(sideComb, layerIdx, mPath,
                            mPath->getBxTimeValue()
                           );

  float denom = CELL_HEIGHT * dVert;
  float tanPhi = num / denom;

  mPath->setTanPhi(tanPhi);

  /*-----------------------------------------------------------------*/
  /*----------------- Horizontal coord. calculation -----------------*/
  /*-----------------------------------------------------------------*/
  float XPos = 0;
  if (mPath->getPrimitive(0)->isValidTime() &&
      mPath->getPrimitive(3)->isValidTime())
    XPos = (mPath->getXCoorCell(0) + mPath->getXCoorCell(3)) / 2;
  else
    XPos = (mPath->getXCoorCell(1) + mPath->getXCoorCell(2)) / 2;

  mPath->setHorizPos( XPos );
}

/**
 * Calcula las distancias de deriva respecto de cada "wire" y la posición
 * horizontal del punto de interacción en cada celda respecto del sistema
 * de referencia de la cámara.
 *
 * La posición horizontal de cada hilo es calculada en el "DTPrimitive".
 */
void PathAnalyzer::calcCellDriftAndXcoor(MuonPath *mPath) {
  //Distancia de deriva en la celda respecto del wire". NO INCLUYE SIGNO.
  float driftDistance;
  float wireHorizPos; // Posicion horizontal del wire.
  float hitHorizPos;  // Posicion del muon en la celda.

  for (int i = 0; i <= 3; i++)
    if (mPath->getPrimitive(i)->isValidTime()) {
      // Drift distance.
      driftDistance = DRIFT_SPEED *
              ( mPath->getPrimitive(i)->getTDCTimeNoOffset() -
                mPath->getBxTimeValue()
              );

      wireHorizPos = mPath->getPrimitive(i)->getWireHorizPos();

      if ( (mPath->getLateralComb())[ i ] == LEFT )
        hitHorizPos = wireHorizPos - driftDistance;
      else
        hitHorizPos = wireHorizPos + driftDistance;

      mPath->setXCoorCell(hitHorizPos, i);
      mPath->setDriftDistance(driftDistance, i);
    }
}

/**
 * Calcula el estimador de calidad de la trayectoria.
 */
void PathAnalyzer::calcChiSquare(MuonPath *mPath) {

  float xi, zi, factor;

  float chi = 0;
  float mu  = mPath->getTanPhi();
  float b   = mPath->getHorizPos();

  const float baseWireYPos = -1.5 * CELL_HEIGHT;

  for (int i = 0; i <= 3; i++)
    if ( mPath->getPrimitive(i)->isValidTime() ) {
      zi = baseWireYPos + CELL_HEIGHT * i;
      xi = mPath->getXCoorCell(i);

      factor = xi - mu*zi - b;
      chi += (factor * factor);
    }

  mPath->setChiSq(chi);
}


/**
 * Este método devuelve cual layer no se está utilizando en el
 * 'LAYER_ARRANGEMENT' cuyo índice se pasa como parámetro.
 * 
 * ¡¡¡ OJO !!! Este método es completamente dependiente de esa macro.
 * Si hay cambios en ella, HAY QUE CAMBIAR EL MÉTODO.
 * 
 *  LAYER_ARRANGEMENTS[MAX_VERT_ARRANG][3] = {
 *    {0, 1, 2}, {1, 2, 3},                       // Grupos consecutivos
 *    {0, 1, 3}, {0, 2, 3}                        // Grupos salteados
 *  };
 */
int PathAnalyzer::getOmittedHit(int idx) {
  
  int ans = -1;
  
  switch (idx) {
    case 0: ans = 3; break;
    case 1: ans = 0; break;
    case 2: ans = 2; break;
    case 3: ans = 1; break;
  }

  return ans;
}

}
