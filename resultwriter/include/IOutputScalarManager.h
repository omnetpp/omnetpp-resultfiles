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

#ifndef __IOUTPUTSCALARMANAGER_H
#define __IOUTPUTSCALARMANAGER_H

#include <stdlib.h>
#include <map>
#include "IResultManager.h"
#include "IStatisticalSummary.h"

/**
 * Interface for recording output scalar data. It is recommended to have only one
 * instance of IOutputScalarManager for the whole simulation, and simulation objects
 * (components) should have <code>recordScalar()</code> methods that delegate to
 * methods of the global IOutputScalarManager instance.
 *
 * IOutputScalarManager methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
class IOutputScalarManager : public IResultManager
{
  public:
    /**
     * Records a numeric scalar result.
     *
     * @param componentPath  full path name of the component that records the statistic.
     *      The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param statisticName  name of the statistic. This should not usually
     *      contain dots.
     * @param attributes  statistic attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    virtual void recordScalar(const char *componentPath, const char *statisticName, double value,
                              StringMap attributes) = 0;

    /**
     * Records a histogram or statistic object. If the object additionally implements
     * the IStatisticalSummary2 and/or IHistogramSummary interfaces, more data are recorded.
     *
     * @param componentPath  full path name of the component that records the statistic.
     *      The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param statisticName  name of the statistic object. This should not usually
     *      contain dots.
     * @param attributes  statistic attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    virtual void recordStatistic(const char *componentPath, const char *statisticName, IStatisticalSummary *statistic, StringMap attributes) = 0;
};

#endif
