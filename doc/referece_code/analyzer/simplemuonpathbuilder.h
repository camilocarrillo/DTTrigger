/**
 * Project:
 * Subproject: tracrecons
 * File name:  simplemuonpathbuilder.h
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
 * Copyright (c) 2015-09-24 Jose Manuel Cela <josemanuel.cela@ciemat.es>
 *
 * For internal use, all rights reserved.
 * *********************************************************************
 */
#ifndef SIMPLEMUONPATHBUILDER_H
#define SIMPLEMUONPATHBUILDER_H

#include "i_task.h"
#include "i_pusher.h"
#include "i_popper.h"
#include "dtprimitive.h"

using namespace CIEMAT;

namespace CMS {

class SimpleMuonPathBuilder : public I_Task {

  public:
    SimpleMuonPathBuilder(I_Popper &inDTPrim, I_Pusher &outMuonPath);
    virtual ~SimpleMuonPathBuilder();

    void run(void);
    void stop(void);

    void setRRobin(bool rr);
    void buildData(void);

  private:
    I_Popper &inDTPrim;
    I_Pusher &outMuonPath;

    bool roundRobin;

    void discoverHorizontalLayout(DTPrimitive *DTprim[4], int *layout);
};

}

#endif
