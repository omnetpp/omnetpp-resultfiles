/*
 * Copyright (c) 2010, Andras Varga and Opensim Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Opensim Ltd. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _XYARRAY_H_
#define _XYARRAY_H_

#include "scavedefs.h"

NAMESPACE_BEGIN

/**
 * ArrayBuilderNode returns the results in this class.
 */
class SCAVE_API XYArray
{
    private:
        double *x;
        double *y;
        int len;
        BigDecimal *xp; // precise x value
        eventnumber_t *evec; // event numbers
    public:
        XYArray(int l, double *xv, double *yv, BigDecimal *xpv = NULL, eventnumber_t *ev = NULL) {len = l; x = xv; y = yv; xp = xpv; evec = ev;}
        ~XYArray() {delete [] x; delete [] y; delete [] xp; delete [] evec;}
        int length() const  {return len;}
        double getX(int i) const  {return (i>=0 && i<len) ? x[i] : 0;}
        double getY(int i) const  {return (i>=0 && i<len) ? y[i] : 0;}
        BigDecimal getPreciseX(int i) const {return ((xp != NULL && i>=0 && i < len) ? xp[i] : BigDecimal::Nil);}
        eventnumber_t getEventNumber(int i) const {return ((evec != NULL && i>=0 && i<len) ? evec[i] : -1);}
        void sortByX();
};

NAMESPACE_END


#endif


