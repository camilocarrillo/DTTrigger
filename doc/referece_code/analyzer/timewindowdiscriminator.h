/**
 * Project:
 * Subproject: tracrecons
 * File name:  timewindowdiscriminator.h
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
 * Copyright (c) 2015-04-15 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef TIMEWINDOWDISCRIMINATOR_H
#define TIMEWINDOWDISCRIMINATOR_H

#include "analtypedefs.h"
#include "i_cyclictwindowaccept.h"
#include "i_filter.h"
#include "dtprimitive.h"

using namespace CIEMAT;

namespace CMS {

/**
 * Esta clase procesa todos los objetos DTPrimitive de entrada, filtrándolos
 * en función de un valor de ventana de rechazo programado.
 */
/*
  It inherits from "I_CyclicTWindowAccept".
  The 2 parameters "currentTime" and "timeWindow" that come from that parent
  class, will be used to determine if a new incoming DTPrimitive
  will be accepted for processing (filtering and accepting/rejecting) or
  will be kept pending until its TDCtime value is within the time-window
  starting on "currentTime" and ending on "currentTime + acceptTimeWindow"

  The value of this time-window and the currentTime increment rate should
  be calibrated to allow this thread to fetch enough DTP's to feed the rest
  of the processes, but few enough (retained in the incoming FIFO) to allow
  the processing of those sent previously.
  In general, the window should be bigger than that used at "SeekTimeBuffers"
  to find time-accepted hits.

  In the case of this simulation (it shouldn't be the same on the FPGA), because
  the step used to increment "currentTime" on the "MultiChannelDataMixer" (in
  charge of changing the global current time) could move the time beyond the
  value of the next pending DTP tdcTime to be processed. In this case, the DTP
  will be left pending, because it goes out the acceptance window, left in a
  "past time" behind the new current time.
  To avoid this, the value for "currentTime" in this class should be set "in
  the past" of the MultiChannelDataMixer one, and the acceptance window here
  much bigger than there.

                  MChDatMix.
                   Current
                    Time
  |              (Main time)                                     (time line)
  |-----------|------||-----------------------------------------|-------->
  0           |      ||                                         |
              |      ||                                         |
              |      ||                                         |
              |   TWDisc.                                       |
              |   Current                                       |
              |    Time                                         |
              |      |                                          |
              |      |<---------------------------------------->|
              |             TWDisc.
              |             Acceptance window
              |                      |
              |<-------------------->|
              |     SeekTimeBuf      |
              |   Acceptance window  |

    In the meantime I think that can be good that the end of the SeekTimeBuffer
    window were the beginning of the TWD acceptance window. This has to be
    tested
  */

class TimeWindowDiscriminator : public I_Filter, public I_CyclicTWindowAccept {

  public:
    typedef struct {
      unsigned int totAccepted;
      unsigned int totDiscarded;
      unsigned int totInvalid;
      unsigned int accepted[NUM_SUPERLAYERS][NUM_LAYERS][NUM_CH_PER_LAYER];
      unsigned int discarded[NUM_SUPERLAYERS][NUM_LAYERS][NUM_CH_PER_LAYER];
    } STATISTICS;

    TimeWindowDiscriminator(I_Popper &dataIn, I_Pusher &dataOut);
    virtual ~TimeWindowDiscriminator();

    void filter(void);
    void reset(void);
    /*
      Por el momento se fija un valor de ventana temporal de discriminación
      global, común para todos los canales
     */
    void setRejectTimeWindow(int tWindow);
    int getRejectTimeWindow(void);

    const STATISTICS* getStatistics(void);

  private:
    int lastTimeStamp[NUM_SUPERLAYERS][NUM_LAYERS][NUM_CH_PER_LAYER];
    int rejectTimeWindow;
    bool thereIsPendingData;
    DTPrimitive *dtPrim;

    /* Contadores para estadísticas */
    STATISTICS stats;
};

}

#endif
