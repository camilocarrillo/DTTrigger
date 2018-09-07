/**
 * Project:
 * Subproject: tracrecons
 * File name:  lockablememory.h
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
#ifndef LOCKABLEMEMORY_H
#define LOCKABLEMEMORY_H

#include "i_object.h"

namespace CMS {

class LockableMemory {

  public:
    LockableMemory(unsigned int rows = 100, unsigned int cols = 100);
    virtual ~LockableMemory();

    void write(I_Object* obj);
    I_Object* read(unsigned int )

    void setAutoLockPage(bool lock);
    bool getAutoLockPage(void);

  private:
    I_Object* memory;
    unsigned int numRows, numCols;
    bool autoLock;

};

}

#endif
