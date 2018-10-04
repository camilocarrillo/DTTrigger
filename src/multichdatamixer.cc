#include "multichdatamixer.h"

#include <cstring>  // Para función "memcpy"
#include <vector>

// Ver "main.cpp"
extern bool orbitDataInserted;
extern bool orbitDataSplitted;

/*
  Channels are labeled following next schema:
    ---------------------------------
    |   6   |   7   |   8   |   9   |
    ---------------------------------
        |   3   |   4   |   5   |
        -------------------------
            |   1   |   2   |
            -----------------
                |   0   |
                ---------
 */
/* Cell's combination, following previous labeling, to obtain every possible
muon's path. Others cells combinations imply non straight paths */
const int MultiChDataMixer::CHANNELS_PATH_ARRANGEMENTS[8][4] = {
  {0, 1, 3, 6}, {0, 1, 3, 7}, {0, 1, 4, 7}, {0, 1, 4, 8},
  {0, 2, 4, 7}, {0, 2, 4, 8}, {0, 2, 5, 8}, {0, 2, 5, 9}
};

/* For each of the previous cell's combinations, this array stores the
associated cell's displacement, relative to lower layer cell, measured in
semi-cell length units */
const int MultiChDataMixer::CELL_HORIZONTAL_LAYOUTS[8][4] = {
  {0, -1, -2, -3}, {0, -1, -2, -1}, {0, -1, 0, -1}, {0, -1, 0, 1},
  {0,  1,  0, -1}, {0,  1,  0,  1}, {0,  1, 2,  1}, {0,  1, 2, 3}
};

//------------------------------------------------------------------
//--- Constructores y destructores
//------------------------------------------------------------------
MultiChDataMixer::MultiChDataMixer(midBuffers_t &channelIn,
                                   I_Pusher &outMuonPath,
                                   TimeWindowDiscriminator &twDisc) :
    chInDummy(), outMuonPath(outMuonPath), twDisc(twDisc)
{
    std::cout<<"Creando un 'MultiChDataMixer'"std::endl;
    setRRobin(false);
    setInChannels(channelIn);
    reset();
}

MultiChDataMixer::~MultiChDataMixer() {
    std::cout<<"Destruyendo un 'MultiChDataMixer'"std::endl;
}

//------------------------------------------------------------------
//--- Métodos públicos
//------------------------------------------------------------------
void MultiChDataMixer::run(void)
{
  while(roundRobin)
    mixData();

  /* Sentencias repetidas para evitar el bloqueo del READER cuando se para el
   * programa. Esto ocurre si los "if(!roundRobin)" hacen que se pare el 
   * "mixData" sin que haya dado tiempo a que se cambie el valor de estos
   * booleanos a "false".
   * En ese caso el READER puede quedarse en el bucle infinito "while" 
   * que tiene para esperar entre 2 eventos consecutivos.
   */ 
  orbitDataInserted = false;
  orbitDataSplitted = false;
  
}

void MultiChDataMixer::stop(void) { setRRobin(false); }
void MultiChDataMixer::setRRobin(bool rr) { roundRobin = rr; }

/**
 * This method implements and algorithm to gather a group of channel, from
 * the whole chamber's channels number, involved in a possible path (for a
 * given base cell channel).
 * Each group gathers 10 channels.
 */
void MultiChDataMixer::mixData(void) {
  
  
if (orbitDataSplitted) {  //*************************************************
  
  /*
    To avoid changing the meaning of the original "current time" in the
    SeekTimeBuffers, and the sense of their window searching time, I will fix
    the current time "in the past" of the real current time in the same amount
    that their seek window width.
    This way the end of the seek window is coincident with the "current time",
    as well as with the beginning of the acceptance window of the
    "TimeWindowDiscriminator".
    The "SeekTimeBuffers" window is in the past, while the
    "TimeWindowDiscriminator" window in the future.

    Be careful that the attribute were configured with the same value than that
    used at the SeekTimeBuffer window size.
  */
//   LOGDEBUG_INT("Current time: ", currentTime);
//   setChCurrentTime(currentTime - seekTimeBufWindowRelBeginning);
//   twDisc.setCurrentTime(currentTime);
  /*
    Runs along the total number of lower layer channel, equivalent to the
    number of operational BTIs
    
    En la nueva versión, se recorren también los índices de SUPERLAYER.
    De esta forma con un sólo MIXER QUE SÓLO HAGA MEZCLAS DE HITS DE UNA
    SUPERLAYER DADA podemos hacer el procesamiento para muchas SL a la vez
    con la misma estructura de componentes.
  */
  for(int baseCh = 0; baseCh < TOTAL_BTI; baseCh++) {
    currentBaseChannel = baseCh;
    selectInChannels(baseCh);
    if (!roundRobin) return;

    /* 
     * Si no hay suficientes datos para construir trazas, saltamos al
     * siguiente canal base. Deberíamos ahorrar mucho tiempo de proceso.
     */
    if ( notEnoughDataInChannels() ) continue;

    for(int pathId = 0; pathId < 8; pathId++) {
      if (!roundRobin) return;
      /* 
       * Si no hay suficientes datos para construir trazas, saltamos al
       * siguiente canal base. Deberíamos ahorrar mucho tiempo de proceso.
       */
      if ( notEnoughDataInChannels() ) continue;
      
      for (int supLayer = 0; supLayer < NUM_SUPERLAYERS; supLayer++) {
        
        /***
         * ¡¡¡¡ OJO !!!!
         * 
         * En esta modalidad multi-superlayer HABRÍA QUE CAMBIAR LOS 
         * SEEKTIMEBUFFER's para que dejen de enviar datos no sólo cuando
         * el tiempo de un HIT esté fuera de la ventana de búsqueda, sino
         * que esta comprobación de tiempos habría de hacerse SUPERLAYER A
         * SUPERLAYER. 
         * Esto supondría que hay que hacer una búsqueda exhaustiva por todo
         * el buffer puesto que, tras un HIT de una SL que ya no esté en la
         * ventana de tiempos programad, podría haber otros HITS DE OTRA SL
         * DISTINTA que sí estuvieran en la ventana.
         * 
         * Con esta arquitectura en la que no se desdoblan los buffers para
         * HITS y se ha programado una ventana de búsqueda temporal de 
         * 89100 ns, equivalente a todo el tiempo de una órbita, NO TIENEN
         * SENTIDO LOS SEEKTIMEBUFFER's tal y como están concebidos.
         * 
         * SE HACE NECESARIO CAMBIARLOS POR BUFFERS (VECTORES) SIMPLES sobre
         * los que se hagan TODAS LAS MEZCLAS posibles de HITS por cada SL,
         * y hay que ESTUDIAR/CAMBIAR EL MECANISMO DE GESTIÓN DE TIEMPOS
         * POR ALGO MÁS SENCILLO.
         */
        resetPrvTDCTStamp();
        mixChannels(supLayer, pathId);
      }
    }
  }
  /*
    New way of doing things: from now on, currentTime is always incremented in
    a fixed amount of time.
  */
  currentTime += forcedTimeInc;
  if (currentTime > MAX_VALUE_OF_TIME){
    currentTime = 0;
    orbitNum++;

  orbitDataInserted = false;
  orbitDataSplitted = false;
  }

  
  std::cout<<"Current time: "<<urrentTime<<std::endl;
  setChCurrentTime(currentTime - seekTimeBufWindowRelBeginning);
  twDisc.setCurrentTime(currentTime);
  
  
} //*************************************************


}

void MultiChDataMixer::reset(void) {
  currentTime                   = 0;
  forcedTimeInc                 = 50;
  orbitNum                      = 0;
  seekTimeBufWindowRelBeginning = 600;
  resetPrvTDCTStamp();
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void MultiChDataMixer::setInChannels(midBuffers_t &channelIn)
{
  for (int lay = 0; lay < NUM_LAYERS; lay++)
    for (int ch = 0; ch < NUM_CH_PER_LAYER; ch++)
      this->channelIn[lay][ch] = channelIn[lay][ch];
}

void MultiChDataMixer::setForcedTimeIncrement(int time) {
  if (time >= 1) forcedTimeInc = time;
}

void MultiChDataMixer::setSeekTimeBufWindowRelBeginning(int time) {
  seekTimeBufWindowRelBeginning = time;
}

bool MultiChDataMixer::isThereDataInAnyChannel(void) {
  bool ans = false;

  for (int lay = 0; lay < NUM_LAYERS; lay++)
    for (int ch = 0; ch < NUM_CH_PER_LAYER; ch++) {
      if ( ! this->channelIn[lay][ch]->isEmpty() ) ans = true;
      break;
    }

  return ans;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
/**
 * Implements a channel muxer, selecting from the overall channel amount, those
 * of them related with a BTI.
 * At this moment, this method implementation is based on calculating channel's
 * relative positions based on 'baseChannel' number.
 * In order to optimize it and aimed to FPGA firmware version, this method
 * should be only a selector from a ROM or equivalent.
 */
void MultiChDataMixer::selectInChannels(int baseChannel) {

  /*
    Channels are labeled following next schema:
            Input Muxer Indexes
      ---------------------------------
      |   6   |   7   |   8   |   9   |
      ---------------------------------
          |   3   |   4   |   5   |
          -------------------------
              |   1   |   2   |
              -----------------
                  |   0   |
                  ---------
   */
  /*
    ****** CAPA 0 ******
   */
  muxInChannels[0] = channelIn[0][baseChannel];
  /*
    ****** CAPA 1 ******
   */
  muxInChannels[1] = channelIn[1][baseChannel];

  if (baseChannel + 1 < NUM_CH_PER_LAYER)
    muxInChannels[2] = channelIn[1][baseChannel + 1];
  else
    muxInChannels[2] = &chInDummy;
  /*
    ****** CAPA 2 ******
   */
  if (baseChannel - 1 >= 0)
    muxInChannels[3] = channelIn[2][baseChannel - 1];
  else
    muxInChannels[3] = &chInDummy;

  muxInChannels[4] = channelIn[2][baseChannel];

  if (baseChannel + 1 < NUM_CH_PER_LAYER)
    muxInChannels[5] = channelIn[2][baseChannel + 1];
  else
    muxInChannels[5] = &chInDummy;
  /*
    ****** CAPA 3 ******
   */
  if (baseChannel - 1 >= 0)
    muxInChannels[6] = channelIn[3][baseChannel - 1];
  else
    muxInChannels[6] = &chInDummy;

  muxInChannels[7] = channelIn[3][baseChannel];

  if (baseChannel + 1 < NUM_CH_PER_LAYER)
    muxInChannels[8] = channelIn[3][baseChannel + 1];
  else
    muxInChannels[8] = &chInDummy;

  if (baseChannel + 2 < NUM_CH_PER_LAYER)
    muxInChannels[9] = channelIn[3][baseChannel + 2];
  else
    muxInChannels[9] = &chInDummy;
}

/**
 * Creates a 'MuonPath' object with a 4-DTPrimitive (one per channel)
 * combination.
 * It builds every possible combination extracting the necessary number of
 * objects from each channel, based on the configuration given during
 * 'PathDataMixer' construction, or after 'setCombination' method call
 */
void MultiChDataMixer::mixChannels(int supLayer, int pathId) {

  vector<DTPrimitive*> data[4];

  /*
    Relative to base cell horizontal cells' positions for current pathId whose
    'DTPrims' are being mixed
  */
  int horizLayout[4];
  memcpy(horizLayout, CELL_HORIZONTAL_LAYOUTS[pathId], 4 * sizeof(int));

  /*
    Internal BTI muxer index identifiers (from 0 to 9) for the 4 cells that
    comprises the current path under analysis.
    These values will be used to obtain the number of 'DTPrim' for each channel
    that should be mixed, to obtain different 'MuonPaths'
  */
  int chIdxForPath[4];
  memcpy(chIdxForPath, CHANNELS_PATH_ARRANGEMENTS[pathId], 4 * sizeof(int));

  // Real amount of values extracted from each channel.
  int numPrimsPerLayer[4] = {0, 0, 0, 0};

  unsigned int canal;
  /*
     Si al menos 2 canales están vacíos, salimos de este método porque no es
     posible construir una combinación.
     Espero que con esta nueva condicion se ahorre tiempo de proceso y basura
     en el log de salida.
   */
  int channelEmptyCnt = 0;
  for (int layer = 0; layer <= 3; layer++) {
    canal = CHANNELS_PATH_ARRANGEMENTS[pathId][layer];

    if (((SeekTimeBuffer*) muxInChannels[canal])->isEmpty()) channelEmptyCnt++;
  }
  if (channelEmptyCnt >= 2) return;

  // Extraemos tantos elementos de cada canal como exija la combinacion
  
  for (int layer = 0; layer <= 3; layer++) {

    canal = CHANNELS_PATH_ARRANGEMENTS[pathId][layer];
    /*
       To avoid infinite loops I get the maximum number of items that will be
       combined, retrieving the number of currently stored elements on each
       buffer.
    */
    unsigned int maxPrimsToBeRetrieved =
                ((SeekTimeBuffer*) muxInChannels[canal])->storedElements();

    /*
      If the number of primitives is zero, in order to avoid that only one
      empty channel avoids mixing data from the other three, we, at least,
      consider one dummy element from this channel.
      In other cases, where two or more channels has zero elements, the final
      combination will be not analyzable (the condition for being analyzable is
      that it has at least three good TDC time values, not dummy), so it will
      be discarded and not sent to the analyzer.
    */
    if (maxPrimsToBeRetrieved == 0) maxPrimsToBeRetrieved = 1;

    /*
      On this new working schema, first of all it's necessary to rollback
      buffer pointers because other execution stage could have left the
      buffer internal pointers on an unknown position
    */
    ((SeekTimeBuffer*) muxInChannels[canal])->rollBack();
    for (unsigned int items = 0; items < maxPrimsToBeRetrieved; items++) {

      DTPrimitive* dtpAux = (DTPrimitive*) (muxInChannels[canal])->pop();
      /*
        I won't allow a whole loop cycle. When a DTPrimitive has an invalid
        time-stamp (TDC value = -1) it means that the buffer is empty or the
        buffer has reached the last element within the configurable time window.
        In this case the loop is broken, but only if there is, at least, one
        DTPrim (even invalid) on the outgoing array. This is mandatory to cope
        with the idea explained in the previous comment block
      */
      if (dtpAux->getTDCTime() < 0 && items > 0) break;
      /*
       * En este nuevo esquema, si el HIT se corresponde con la SL sobre la
       * que se están haciendo las mezclas, se envía al buffer intermedio
       * de mezclas.
       * En caso contrario, se envía una copia en blanco "inválida" para que
       * la mezcla se complete, como ocurría en el caso de una sola SL.
       * 
       * En este caso, un poco chapuza, habrá bastantes casos en los que 
       * se hagan mezclas inválidas. Por ello, la verificación que hay más
       * adelante, en la que se comprueba si el segmento "es analizable"
       * antes de ser enviado a la cola de salida, ES IMPRESCINDIBLE.
       */
      if (dtpAux->getSuperLayerId() == supLayer) 
        data[layer].push_back(dtpAux);
      else
        data[layer].push_back( new DTPrimitive() );
        
      numPrimsPerLayer[layer]++;
    }
  }

  std::cout<<"'MultiChDataMixer::mixChannels' mezcla completa: "<<path<<std::endl;

  DTPrimitive *ptrPrimitive[4];
  /*
    Realizamos las diferentes combinaciones y las enviamos a las fifo de
    salida
  */
  int chIdx[4];

  for (chIdx[0] = 0; chIdx[0] < numPrimsPerLayer[0]; chIdx[0]++)
    for (chIdx[1] = 0; chIdx[1] < numPrimsPerLayer[1]; chIdx[1]++)
      for (chIdx[2] = 0; chIdx[2] < numPrimsPerLayer[2]; chIdx[2]++)
        for (chIdx[3] = 0; chIdx[3] < numPrimsPerLayer[3]; chIdx[3]++) {

          if (!roundRobin) return;
          /*
            Creamos una copia del objeto para poder manipular cada copia en
            cada hilo de proceso de forma independiente, y poder destruirlas
            cuando sea necesario, sin depender de una única referencia a lo
            largo de todo el código.
          */
	  std::cout<<"MultiChDataMixer::mixChannelsCombinando primitivas."<<std::endl;

          for (int i = 0; i <= 3; i++) {
            ptrPrimitive[i] = new DTPrimitive( (data[i])[chIdx[i]] );
	    std::cout<<"Capa: "<<ptrPrimitive[i]->getLayerId()<<" Canal: "<<ptrPrimitive[i]->getChannelId()<<" TDCTime: "<<ptrPrimitive[i]->getTDCTime()<<std::endl;
          }

          MuonPath *ptrMuonPath = new MuonPath(ptrPrimitive);
          ptrMuonPath->setCellHorizontalLayout(horizLayout);

          /*
            This new version of this code is redundant with PathAnalyzer code,
            where every MuonPath not analyzable is discarded.
            I insert this discarding mechanism here, as well, to avoid inserting
            not-analyzable MuonPath into the candidate FIFO.
            Equivalent code must be removed in the future from PathAnalyzer, but
            it the mean time, at least during the testing state, I'll preserve
            both.
            Code in the PathAnalyzer should be doing nothing now.
          */
          if (ptrMuonPath->isAnalyzable()) {
            /*
              This is a very simple filter because, during the tests, it has been
              detected that many consecutive MuonPaths are duplicated mainly due
              to buffers empty (or dummy) that give a TDC time-stamp = -1
              With this filter, I'm removing those consecutive identical
              combinations.

              If duplicated combinations are not consecutive, they won't be
              detected here
            */
            if ( !isEqualComb2Previous(ptrPrimitive) ) {
		std::cout<<"'MultiChDataMixer::mixChannels'. Pushing 'MuonPath'"<<std::endl;

              /* Este código es una chapuza para depuraciones */
	      std::cout<<"'MuonPath' analyzable. TDC Time's: "
		       <<ptrMuonPath->getPrimitive(0)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(1)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(2)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(3)->getTDCTime()<<std::endl;
		
		/*
                Después de este 'push' no se debe utilizar el 'ptrMuonPath' puesto
                que el 'thread' del analizador puede ser más rápido que este
                'thread' local, llegando a destruir el 'ptrMuonPath' antes que el
                local haga uso de alguno de sus métodos, lo que deriva en un
                bonito 'segment violation' al acceder a un espacio de memoria que ha
                sido eliminado
              */
              ptrMuonPath->setBaseChannelId(currentBaseChannel);
              outMuonPath.push( ptrMuonPath );

	      std::cout<<"'MultiChDataMixer::mixChannels'. Pushed 'MuonPath'"<<std::endl;
            }
            else delete ptrMuonPath;
          }
          else {
	      std::cout<<"PRIM not analyzable. TDC Time's: "
		       <<ptrMuonPath->getPrimitive(0)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(1)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(2)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(3)->getTDCTime()<<std::endl;
		  
	      delete ptrMuonPath;
          }
        }
  /*
    Nos cepillamos la memoria ocupada por las primitivas que hemos extraído del
    "popper". Es responsabilidad del "popper" enviar copias de los objetos, en
    vez de los originales, si considera que es necesario volver a enviar alguna
    de ellas.
    Esto, al final, es necesario hacerlo así en este y en otros componentes por
    el criterio de manda-güevos, porque, si todos los objetos son sólo
    referencias, pero no duplicados, puede ocurrir que, en un momento dado, el
    componente responsable de hacer "expirar" un DTPrimitive (porque su
    timestamp esté fuera de la ventana temporal de búsqueda) lo elimine, pero
    al resto de componentes no les haya dado a finalizar su procesamiento, por
    lo que nos encontraríamos con un bonito puntero NULL en algún lugar de la
    Mancha y la habríamos liado parda.
  */
  for (int layer = 0; layer <= 3; layer++) {
    int numData = data[layer].size();
    for (int i = 0; i < numData; i++) {
      delete (DTPrimitive*) ((data[layer])[i]);
    }
    data[layer].clear();
  }
  
  
  

}

/** \brief Establece el "currentTime" en los buffers de entrada al mezclador.
 *
 * \param None
 * \return None
 */
void MultiChDataMixer::setChCurrentTime(int time) {

    std::cout<<"Cambiando CHANNEL current Time: "<<time<<std::endl;

  for (int lay = 0; lay < NUM_LAYERS; lay++)
    for (int ch = 0; ch < NUM_CH_PER_LAYER; ch++)
      this->channelIn[lay][ch]->setCurrentTime(time);

}

void MultiChDataMixer::resetPrvTDCTStamp(void) {
  for (int i = 0; i <= 3; i++) prevTDCTimeStamps[i] = -1;
}

bool MultiChDataMixer::isEqualComb2Previous(DTPrimitive *dtPrims[4]) {
  bool answer = true;

  for (int i = 0; i <= 3; i++)
    if (prevTDCTimeStamps[i] != dtPrims[i]->getTDCTime()) {
      answer = false;
      for (int j = 0; j <= 3; j++) {
        prevTDCTimeStamps[j] = dtPrims[j]->getTDCTime();
      }
      break;
  }

  return answer;
}

/**
 * Returns TRUE when given certain base channel, at least 3 layers has hits
 * (their buffers are not empty) so it is possible to mix data forming tracks.
 */
bool MultiChDataMixer::notEnoughDataInChannels(void) {

  // Indicadores de "layer" empty.
  bool lEmpty[4];

  lEmpty[0] = ((SeekTimeBuffer*) muxInChannels[0])->isEmpty();

  lEmpty[1] = ((SeekTimeBuffer*) muxInChannels[1])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[2])->isEmpty();

  lEmpty[2] = ((SeekTimeBuffer*) muxInChannels[3])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[4])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[5])->isEmpty();

  lEmpty[3] = ((SeekTimeBuffer*) muxInChannels[6])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[7])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[8])->isEmpty() &&
              ((SeekTimeBuffer*) muxInChannels[9])->isEmpty();

  /* Si al menos 2 layers están vacías, no se puede construir mezcla con
   * posible traza.
   */
  if ( (lEmpty[0] && lEmpty[1]) || (lEmpty[0] && lEmpty[2]) ||
       (lEmpty[0] && lEmpty[3]) || (lEmpty[1] && lEmpty[2]) ||
       (lEmpty[1] && lEmpty[3]) || (lEmpty[2] && lEmpty[3]) )
    return true;
  else
    return false;

}
