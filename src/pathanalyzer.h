#ifndef PATHANALYZER_H
#define PATHANALYZER_H

#include "L1Trigger/DTTrigger/src/muonpath.h"
#include "L1Trigger/DTTrigger/src/analtypedefs.h"

#define MAX_VERT_ARRANG 4

class PathAnalyzer {
  typedef struct {
    unsigned int mpAnalized;
    unsigned int mpNull;
    unsigned int mpNotAnalized;
    unsigned int mpWithValidPath;
    unsigned int mpNoValidPath;
    unsigned int mpHighQ;
    unsigned int mpHighQGhost;
    unsigned int mpLowQ;
    unsigned int mpLowQGhost;
  } STATISTICS;

  typedef struct {
    bool latQValid;
    int  bxValue;
  } PARTIAL_LATQ_TYPE;

  typedef struct {
    bool valid;
    int bxValue;
    /* Este campo se utiliza para aquellos casos de 4 hits combinados
     * pero en los que sólo 3 de ellos se ajustan a una recta.
     * En ese caso, dentro de este campo se almacena el índice de la 
     * layer del hit que no funciona bien.
     * Sino, DEBE CONTENER VALOR -1.
     */
    int invalidateHitIdx;
    MP_QUALITY quality;
  } LATQ_TYPE;

  public:
  PathAnalyzer(MuonPath &inMuonPath, MuonPath &outValidMuonPath);
  virtual ~PathAnalyzer();
    
    void run(void);
    void stop(void);
    MuonPath analyze(MuonPath *mPath);
    void reset(void);

    void setBXTolerance(int t);
    int getBXTolerance(void);

    void setChiSquareThreshold(float ch2Thr);
    
    void setMinimumQuality(MP_QUALITY q);
    MP_QUALITY getMinimumQuality(void);

    const STATISTICS* getStatistics(void);

  private:
    MuonPath &inMuonPath;
    MuonPath &outValidMuonPath;

    float chiSquareThreshold;

    /* Combinaciones verticales de 3 celdas sobre las que se va a aplicar el
       mean-timer */
    static const int LAYER_ARRANGEMENTS[MAX_VERT_ARRANG][3];

    /* El máximo de combinaciones de lateralidad para 4 celdas es 16 grupos
       Es feo reservar todo el posible bloque de memoria de golpe, puesto que
       algunas combinaciones no serán válidas, desperdiciando parte de la
       memoria de forma innecesaria, pero la alternativa es complicar el
       código con vectores y reserva dinámica de memoria y, ¡bueno! ¡si hay
       que ir se va, pero ir p'a n'á es tontería! */
    LATERAL_CASES lateralities[16][4];
    LATQ_TYPE latQuality[16];

    int totalNumValLateralities;
    /* Posiciones horizontales de cada celda (una por capa), en unidades de
       semilongitud de celda, relativas a la celda de la capa inferior
       (capa 0). Pese a que la celda de la capa 0 siempre está en posición
       0 respecto de sí misma, se incluye en el array para que el código que
       hace el procesamiento sea más homogéneo y sencillo */
    int cellLayout[4];
    int bxTolerance;
    MP_QUALITY minQuality;

    /* Contadores para estadísticas */
    STATISTICS stats;

    void setCellLayout(const int layout[4]);
    void buildLateralities(void);
    bool isStraightPath(LATERAL_CASES sideComb[4]);

    /* Determina si los valores de 4 primitivas forman una trayectoria
       Los valores tienen que ir dispuestos en el orden de capa:
       0    -> Capa más próxima al centro del detector,
       1, 2 -> Siguientes capas
       3    -> Capa más externa */
    void evaluatePathQuality(MuonPath *mPath);
    void evaluateLateralQuality(int latIdx, MuonPath *mPath,
                                LATQ_TYPE *latQuality);
    /* Función que evalua, mediante el criterio de mean-timer, la bondad
       de una trayectoria. Involucra 3 celdas en 3 capas distintas, ordenadas
       de abajo arriba siguiendo el índice del array.
       Es decir:
            0-> valor temporal de la capa inferior,
            1-> valor temporal de la capa intermedia
            2-> valor temporal de la capa superior
       Internamente implementa diferentes funciones según el paso de la
       partícula dependiendo de la lateralidad por la que atraviesa cada
       celda (p. ej.: LLR => Left (inferior); Left (media); Right (superior))

       En FPGA debería aplicarse la combinación adecuada para cada caso,
       haciendo uso de funciones que generen el código en tiempo de síntesis,
       aunque la función software diseñada debería ser exportable directamente
       a VHDL */
    void validate(LATERAL_CASES sideComb[3], int layerIndex[3],
                  MuonPath* mPath, PARTIAL_LATQ_TYPE *latq);

    int eqMainBXTerm(LATERAL_CASES sideComb[2], int layerIdx[2],
                     MuonPath* mPath);

    int eqMainTerm(LATERAL_CASES sideComb[2], int layerIdx[2], MuonPath* mPath,
                   int bxValue);

    void getLateralCoeficients(LATERAL_CASES sideComb[2], int *coefs);
    bool sameBXValue(PARTIAL_LATQ_TYPE *latq);

    void calculatePathParameters(MuonPath *mPath);
    void calcTanPhiXPosChamber  (MuonPath *mPath);
    void calcCellDriftAndXcoor  (MuonPath *mPath);
    void calcChiSquare          (MuonPath *mPath);

    void calcTanPhiXPosChamber3Hits(MuonPath *mPath);
    void calcTanPhiXPosChamber4Hits(MuonPath *mPath);
    
    int getOmittedHit(int idx);
};



#endif
