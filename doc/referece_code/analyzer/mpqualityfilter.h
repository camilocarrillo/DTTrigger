/**
 * Project:
 * Subproject: tracrecons
 * File name:  mpqualityfilter.h
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
 * Copyright (c) 2015-07-02 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef MPQUALITYFILTER_H
#define MPQUALITYFILTER_H

#include "analtypedefs.h"
#include "i_filter.h"

using namespace CIEMAT;

namespace CMS {

class MPQualityFilter : public I_Filter {

  public:
    MPQualityFilter(I_Popper &dataIn, I_Pusher &dataOut);
    virtual ~MPQualityFilter();

    void filter(void);

    void setMinQuality(MP_QUALITY q);
    MP_QUALITY getMinQuality(void);

  private:
    MP_QUALITY quality;
};

}

#endif
