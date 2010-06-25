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

#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <float.h>
#include "scavedefs.h"
#include "scaveutils.h"

NAMESPACE_BEGIN

/**
 * Class for collecting statistical data of the result
 * used in several places in Scave.
 */
class SCAVE_API Statistics
{
    private:
        long _count;
        double _min;
        double _max;
        double _sum;
        double _sumSqr;

    public:
        Statistics() : _count(0), _min(POSITIVE_INFINITY), _max(NEGATIVE_INFINITY), _sum(0.0), _sumSqr(0.0) {}
        Statistics(long count, double min, double max, double sum, double sumSqr)
            :_count(count), _min(min), _max(max), _sum(sum), _sumSqr(sumSqr) {}

        long getCount() const { return _count; }
        double getMin() const { return _min; }
        double getMax() const { return _max; }
        double getSum() const { return _sum; }
        double getSumSqr() const { return _sumSqr; }
        double getMean() const { return _count == 0 ? NaN : _sum / _count; }
        double getStddev() const { return sqrt(getVariance()); }
        double getVariance() const;

        void collect(double value);
        void adjoin(const Statistics &other);
};

inline double Statistics::getVariance() const
{
    if (_count >= 1)
    {
        double var = (_sumSqr - _sum*_sum/_count)/(_count-1);
        return var < 0 ? 0 : var;
    }
    else
        return NaN;
}

inline void Statistics::collect(double value)
{
    _count++;
    _min = (_min < value ? _min : value);
    _max = (_max > value ? _max : value);
    _sum += value;
    _sumSqr += value * value;
}

inline void Statistics::adjoin(const Statistics &other)
{
    _count += other._count;
    _min = (_min < other._min ? _min : other._min);
    _max = (_max > other._max ? _max : other._max);
    _sum += other._sum;
    _sumSqr += other._sumSqr;
}

NAMESPACE_END


#endif
