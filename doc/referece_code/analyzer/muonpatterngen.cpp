#include "muonpatterngen.h"

#include "genfun.h"
#include "logmacros.h"
#include <math.h>
#include <cstring>
#include <iostream>

using namespace std;

namespace CMS {

// const float MuonPatternGen::slVertOffset[NUM_SUPERLAYERS] = {0, 850, 1050};
// const float MuonPatternGen::slVertOffset[NUM_SUPERLAYERS] = {0, 237, 474};
// const float MuonPatternGen::slVertOffset[NUM_SUPERLAYERS] = {0, 237};
// const float MuonPatternGen::slVertOffset[NUM_SUPERLAYERS] = {0, 52};
const float MuonPatternGen::slVertOffset[NUM_SUPERLAYERS] = {0};
// const float MuonPatternGen::slHorizOffset[NUM_SUPERLAYERS]= {0, 0, 0};
// const float MuonPatternGen::slHorizOffset[NUM_SUPERLAYERS]= {0, 0};
const float MuonPatternGen::slHorizOffset[NUM_SUPERLAYERS]= {0};

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
MuonPatternGen::MuonPatternGen() {
  clear();
}

MuonPatternGen::~MuonPatternGen() {}

//------------------------------------------------------------------
//--- Metodos publicos
//------------------------------------------------------------------
/**
 * Construye los patrones de trazas de muones tomando como sistema de
 * referencia aquel dispuesto sobre el hilo de la celda base de la superlayer
 * mas interna del detector.
 *
 * Tiene en cuenta la separacion vertical y horizontal entre superlayers.
 * El numero de las mismas y los diversos parametros de trabajo estan definidos
 * mediante constantes y macros.
 */
int MuonPatternGen::buildPatterns(bool evalLateralities) {

  float delta    = 0;
  float crossP_X = 0;

  int pattern[NUM_SUPERLAYERS][4] = {};
  int laterality[NUM_SUPERLAYERS][4] = {};
  clear();

  withLateralites = evalLateralities;

#ifdef __DEBUG__
unsigned long totalangles  = (ANGLE_MAX - ANGLE_MIN) / ANGLE_STEP;
unsigned long totaloffsets = (CELL_LENGTH - XOFFSET_STEP) / XOFFSET_STEP;

unsigned long percentcompleted = 0;
unsigned long currentpercent   = 0;
unsigned long currentcycle = 0;
unsigned long totalcycles  = totalangles * totaloffsets * 4 * NUM_SUPERLAYERS;

LOGDEBUG_UINT("Total patterns for testing: ",totalcycles);

#endif

  // Barrido en angulo
  for (float alfa = ANGLE_MIN; alfa <= ANGLE_MAX; alfa += ANGLE_STEP) {
    /*
     * Barrido horizontal de la celda inferior de izquierda a derecha.
     *
     * Se evitan los valores extremos de celda (+- CELL_SEMILENGTH) porque
     * generan combinaciones extragnas sin sentido. Por otro lado, considerar
     * estos valores extremos justo en la frontera entre celdas tampoco tiene
     * muchos sentido a la hora de calcular patrones de traza, que no son mas
     * que aproximaciones groseras a la trayectoria real de la particula.
     */
    for (float xOffset = -CELL_SEMILENGTH + XOFFSET_STEP;
               xOffset <  CELL_SEMILENGTH;
               xOffset += XOFFSET_STEP)
    {
      // Barrido por las diferentes superLayers
      for (int sl = 0; sl < NUM_SUPERLAYERS; sl++) {
        // Barrido por las layers de una superlayer
        for (int layer = 0; layer <= 3; layer++) {
#ifdef __DEBUG__
currentcycle++;

currentpercent = currentcycle * 100 / totalcycles;

if (percentcompleted + 9 < currentpercent) {
  LOGDEBUG_UINT("Current cycle: ", currentcycle);
  LOGDEBUG_UINT("Current percent: ", currentpercent);
  percentcompleted = currentpercent;
}

#endif
          /*
           * Punto de corte (X) a la altura del los "wires" de la (sl,layer)
           * dada.
           */
          crossP_X = (layer * CELL_HEIGHT + slVertOffset[sl]) / tan(alfa) +
                     xOffset - slHorizOffset[sl];
          /*
           * Calculo del desplazamiento de la posicion de un wire, relativo al
           * wire de la celda base, en unidades de semilongitud de celda.
           * Debido al desplazamiento relativo de las capas pares respecto de
           * las impares, de una semicelda, el tratamiento para calcular el
           * desplazamiento relativo de forma aritmetica sencilla, es diferente
           * para las capas alternas.
           * El calculo principal se hace en valor absoluto, y luego corrijo el
           * signo dependiendo del lado del sistema de referencia por el que
           * pase la trayectoria.
           */
          if (layer % 2 == 0) {                // Layer par
             delta = (crossP_X + (sgn(crossP_X)*CELL_SEMILENGTH)) / CELL_LENGTH;

            pattern[sl][layer] = sgn(crossP_X) * floor(fabs(delta)) * 2 ;
          }
          else {                               // Layer impar
            delta = crossP_X / CELL_LENGTH;
            pattern[sl][layer] = sgn(crossP_X) * (floor(fabs(delta)) * 2 + 1);
          }

          if (evalLateralities)
            laterality[sl][layer] = calculateLaterality(delta);
        }
      }
      /*
       * Filtrado de patrones. Si un patron ya existe, no se almacena
       * nuevamente. Se realiza un prefiltrado basico comparando con la
       * combinacion inmediatamente anterior para acelerar la ejecucion.
       */
      if ( !existsPattern(pattern, laterality) )
        addPattern(pattern, laterality);
    }
  }

  return patterns.size();
}
/**
 * Muestra en consola la lista de patrones.
 */
void MuonPatternGen::printPatterns(void) {

  for (unsigned int np = 0; np < patterns.size(); np++) {
//    cout << "Pattern: ";
    for (int i = 0; i < 4 * NUM_SUPERLAYERS; i++)
      cout << patterns[np][i] << " ";

    if (withLateralites) {
//      cout << "Lateralities:    ";
      for (int i = 0; i < 4 * NUM_SUPERLAYERS; i++)
        cout << lateralities[np][i] << " ";
    }
    cout << endl;

//    cout << "#" << statistics[np] << endl;
    cout << endl;
  }
}

//------------------------------------------------------------------
//--- Metodos privados
//------------------------------------------------------------------
/*
 * Limpia los contenedores internos asegurando que no deja elementos de memoria
 * ocupados.
 */
void MuonPatternGen::clear(void) {

  unsigned int i;

  for (i = 0; i < patterns.size(); i++) {
    delete [] patterns.back();
    patterns.pop_back();
  }
  for (i = 0; i < lateralities.size(); i++) {
    delete [] lateralities.back();
    lateralities.pop_back();
  }

  patterns.clear();
  lateralities.clear();
  statistics.clear();
}
/*
 * Calcula la lateralidad respecto del hilo, de la trayectoria dada, a partir
 * del valor del discriminador y de la paridad de la capa.
 */
LATERAL_CASES MuonPatternGen::calculateLaterality(float value) {
  LATERAL_CASES lat;
  float discrim = fabs(value - int(value));

  if (value >= 0) {
    if (discrim < 0.5) lat = LEFT;
    else               lat = RIGHT;
  }
  else {
    if (discrim < 0.5) lat = RIGHT;
    else               lat = LEFT;
  }

  return lat;
}
/*
 * Busca si la combinacion proporcionada ya existe en el array interno de
 * patrones.
 * La búsqueda es por fuerza bruta. No es la forma más adecuada, pero se
 * programa facilmente. Ademas no es critica puesto que solo se deberian
 * calcular los patrones una unica vez, al comiezo de la ejecucion. O en FPGA
 * en tiempo de sintesis.
 */
bool MuonPatternGen::existsPattern(int pat[][4], int later[][4]) {

  bool exists = false;

  for (int np = patterns.size() - 1; np >= 0; np--)
    if (patterns[np] != NULL) {
      if (withLateralites) {
        if (memcmp(pat,   patterns[np],     PATTERN_SIZE) == 0 &&
            memcmp(later, lateralities[np], PATTERN_SIZE) == 0)
        {
          exists = true;
          statistics[np]++;
          break;
        }
      }
      else {
        if (memcmp(pat,   patterns[np],     PATTERN_SIZE) == 0) {
          exists = true;
          statistics[np]++;
          break;
        }
      }
    }

  return exists;
}
/*
 * Agrega un patron a la lista interna. No comprueba si existe o no.
 */
void MuonPatternGen::addPattern(int pat[][4], int later[][4]) {

  int *flatPattern = new int[PATTERN_SIZE];
  memcpy(flatPattern, pat, PATTERN_SIZE);
  patterns.push_back(flatPattern);

  if (withLateralites) {
    int *flatLateralities = new int[PATTERN_SIZE];
    memcpy(flatLateralities, later, PATTERN_SIZE);
    lateralities.push_back(flatLateralities);
  }

  statistics.push_back(true);
  statistics[statistics.size() - 1] = 1;
}

}
