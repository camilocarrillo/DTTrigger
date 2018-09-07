#ifndef MUONPATTERNGEN_H
#define MUONPATTERNGEN_H

#define ANGLE_MAX       ((float) 2.7950)   // En rads
#define ANGLE_MIN       ((float) 0.3465)   // En rads
#define ANGLE_STEP      ((float) 0.0001)   // En rads
#define XOFFSET_STEP    ((float) 0.005 )   // En mm
#define YOFFSET_CENTER  ((float) 4186  )   // En mm

#include "analtypedefs.h"
#include "constants.h"
#include <vector>

#define PATTERN_SIZE (4*NUM_SUPERLAYERS*sizeof(int))

namespace CMS {

class MuonPatternGen {

  public:
    MuonPatternGen();
    virtual ~MuonPatternGen();

    int buildPatterns(bool evalLateralities = true);
    void printPatterns(void);

  private:
    /*
     * All of these storages use their indexes to correlate joint information.
     * i.e.: For a given index, lateralities and statistics associated to
     * certain pattern, occupy the same position on their respective vectors.
     */
    std::vector<int*> patterns;     // Pattern storage
    std::vector<int*> lateralities; // Patterns associated lateralities storage
    /*
     * This vector stores the number of times certain pattern has been repeated
     * during patterns' calculation.
     */
    std::vector<int> statistics;

    bool withLateralites;
    /*
     * Offsets entre la primera superlayer y la dada por el i≠ndice del array.
     * Tambien se considera la SL-0 consigo misma porque simplifica el codigo.
     * Este offset es la distancia vertical entre los hilos de cada layer
     * inferior, en "mm".
     * Se consideran para pruebas 2 superlayers, separadas la distancia en una
     * c√°mara
     */
    static const float slVertOffset[NUM_SUPERLAYERS];
    /*
     * El offset horizontal se considera, tambien, respecto del wire de la
     * celda inferior, en la layer mas inferior de todos (centro del cono de
     * trayectorias). Tambien esta dado en "mm".
     */
    static const float slHorizOffset[NUM_SUPERLAYERS];

    void clear(void);
    LATERAL_CASES calculateLaterality(float value);
    bool existsPattern(int pat[][4], int later[][4]);
    void addPattern(int pat[][4], int later[][4]);
};

}
#endif
