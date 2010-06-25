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

#ifndef __ISTATISTICALSUMMARY_H
#define __ISTATISTICALSUMMARY_H


/**
 * Reporting interface for basic univariate statistics.
 *
 * @see IHistogramSummary
 *
 * @author Andras
 */
class IStatisticalSummary
{
  public:
    /**
     * Virtual destructor.
     */
    virtual ~IStatisticalSummary() {}

     /**
     * Returns the <a href="http://www.xycoon.com/arithmetic_mean.htm">
     * arithmetic mean </a> of the available values
     * @return The mean or NaN if no values have been added.
     */
    virtual double getMean() = 0;

    /**
     * Returns the variance of the available values.
     * @return The variance, NaN if no values have been added
     * or 0.0 for a single value set.
     */
    virtual double getVariance() = 0;

    /**
     * Returns the standard deviation of the available values.
     * @return The standard deviation, NaN if no values have been added
     * or 0.0 for a single value set.
     */
    virtual double getStandardDeviation() = 0;

    /**
     * Returns the maximum of the available values
     * @return The max or NaN if no values have been added.
     */
    virtual double getMax() = 0;

    /**
    * Returns the minimum of the available values
    * @return The min or NaN if no values have been added.
    */
    virtual double getMin() = 0;

    /**
     * Returns the number of available values
     * @return The number of available values
     */
    virtual long getN() = 0;

    /**
     * Returns the sum of the values that have been added to Univariate.
     * @return The sum or NaN if no values have been added
     */
    virtual double getSum() = 0;

    /**
     * Returns the squared sum of the values that have been added to Univariate.
     * @return The squared sum or NaN if no values have been added
     *
     * Note: this field is necessary if one wants to completely restore the
     * statistics calculation object.
     */
    virtual double getSqrSum() = 0;
};

#endif
