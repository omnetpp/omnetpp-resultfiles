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

#ifndef _WINDOWAVG_H_
#define _WINDOWAVG_H_

#include "commonnodes.h"

NAMESPACE_BEGIN


/**
 * Processing node which performs window average. Replaces every batch
 * of values with their means.
 */
class SCAVE_API WindowAverageNode : public FilterNode
{
    protected:
        int winsize;
        Datum *array;

    public:
        WindowAverageNode(int windowSize);
        virtual ~WindowAverageNode();
        virtual bool isReady() const;
        virtual void process();
};

class SCAVE_API WindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "winavg";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};

/**
 *
 */
class SCAVE_API TimeWindowAverageNode : public FilterNode
{
    protected:
        simultime_t winsize;
        simultime_t win_end;
        double sum;
        long count;
        void collect(const Datum &d) { sum += d.y; count++; }
        void outputWindowAverage();
        bool inCurrentWindow(const Datum &d) const { return (!d.xp.isNil() && d.xp < win_end) || d.x < win_end.dbl(); }
        void moveWindow(const Datum &d);

    public:
        TimeWindowAverageNode(simultime_t windowSize);
        virtual ~TimeWindowAverageNode();
        virtual bool isFinished() const;
        virtual bool isReady() const;
        virtual void process();
};

inline void TimeWindowAverageNode::outputWindowAverage()
{
    if (count > 0)
    {
        Datum d;
        d.x = win_end.dbl();
        d.xp = win_end;
        d.y = sum / count;
        out()->write(&d, 1);
        sum = 0.0;
        count = 0;
    }
}

inline void TimeWindowAverageNode::moveWindow(const Datum &d)
{
    BigDecimal x = d.xp.isNil() ? BigDecimal(d.x) : d.xp;
    if (x < win_end + winsize)
        win_end += winsize;
    else
    {
        BigDecimal k = floor(x / winsize);
        win_end = (k.dbl() * winsize) + winsize;
    }
}

class SCAVE_API TimeWindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "timewinavg";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};


NAMESPACE_END


#endif


