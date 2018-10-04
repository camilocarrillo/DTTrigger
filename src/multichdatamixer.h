#ifndef MULTICHDATAMIXER_H
#define MULTICHDATAMIXER_H

#include "analtypedefs.h"
#include "muonpath.h"

class MultiChDataMixer {

  public:
    MultiChDataMixer(midBuffers_t &channelIn, I_Pusher &outMuonPath,
                     TimeWindowDiscriminator &twDisc);

    virtual ~MultiChDataMixer();

    void run(void);
    void stop(void);
    void setRRobin(bool rr);
    void mixData(void);
    void reset(void);

    void setInChannels(midBuffers_t &channelIn);
    void setForcedTimeIncrement(int time);

    void setSeekTimeBufWindowRelBeginning(int time);
    bool isThereDataInAnyChannel(void);

  private:
    midBuffers_t   channelIn;
    SeekTimeBuffer chInDummy;
    I_Popper *muxInChannels[10];
    I_Pusher &outMuonPath;
    /*
      WARNING !!! This pointer is only for a fast workaround a to avoid
      creating a new component in charge or managing the global time clock,
      which must conduct the temporal sequence of different parts of the
      algorithm.
      It SHOULD BE ONLY USED for setting the current time into the
      "TimeWindowDiscriminator".
      I know, it's not a good programming style, but this is what it is.
    */
    TimeWindowDiscriminator &twDisc;

    int seekTimeBufWindowRelBeginning;

    bool roundRobin;

    int prevTDCTimeStamps[4];
    int currentTime, forcedTimeInc;
    unsigned int orbitNum;
    
    int currentBaseChannel;

    /* BTI groups of cells comprising every possible combination that gives
       one potentially straight line (a candidate path) */
    static const int CHANNELS_PATH_ARRANGEMENTS[8][4];
    /* Horizontal displacements (measured in half-cell units respect lower
       layer base cell --cell 0--) of each group of cells candidates to form
       an potential path */
    static const int CELL_HORIZONTAL_LAYOUTS[8][4];

    /* From the whole amount of chamber channels, selects a combination of
       channel associated to a one and only base channel. This base channel
       is located at the inner most layer of the chamber (the lower layer) */
    void selectInChannels(int baseChannel);
    void mixChannels(int supLayer, int pathId);

    void setChCurrentTime(int time);
    bool isEqualComb2Previous(DTPrimitive *dtPrims[4]);
    void resetPrvTDCTStamp(void);
    bool notEnoughDataInChannels(void);
};


#endif
