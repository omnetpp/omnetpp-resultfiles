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

#ifndef _STDDEV_H_
#define _STDDEV_H_

#include "commonnodes.h"
#include "xyarray.h"

NAMESPACE_BEGIN


/**
 * Calculates min, max, mean, stddev.
 */
class SCAVE_API StddevNode : public SingleSinkNode
{
    protected:
         long num_vals;
         double min_vals, max_vals;
         double sum_vals, sqrsum_vals;

    protected:
        virtual void collect(double val);

    public:
        StddevNode();
        virtual ~StddevNode();
        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;

        virtual long getCount() const     {return num_vals;}
        virtual double getSum() const     {return sum_vals;}
        virtual double getSqrSum() const  {return sqrsum_vals;}
        virtual double getMin() const     {return min_vals;}
        virtual double getMax() const     {return max_vals;}
        virtual double getMean() const    {return num_vals ? sum_vals/num_vals : 0.0;}
        virtual double getStddev() const;
        virtual double getVariance() const;
};


class SCAVE_API StddevNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *getName() const {return "stddev";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

NAMESPACE_END


#endif


