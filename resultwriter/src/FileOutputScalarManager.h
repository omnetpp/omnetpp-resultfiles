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

#ifndef __FILOUTPUTSCALARMANAGER_H
#define __FILOUTPUTSCALARMANAGER_H

#include <stdlib.h>
#include <map>
#include <string>
#include "OutputFileManager.h"
#include "IOutputScalarManager.h"
#include "IStatisticalSummary2.h"
#include "IHistogramSummary.h"

/**
 * An output scalar manager that writes OMNeT++ scalar (".sca") files.
 *
 * This class does not support filtering (of scalars or recorded data),
 * this functionality may be added via subclasses.
 *
 * @author Andras
 */
class FileOutputScalarManager : public OutputFileManager, public IOutputScalarManager
{
  public:
    static const int FILE_VERSION = 2;

  protected:
    std::string runID;
    StringMap runAttributes;
    std::string fileName;
    std::ofstream *out;

  public:
    FileOutputScalarManager();
    FileOutputScalarManager(const char *fileName);

    ~FileOutputScalarManager();

    void open(const char *runID, StringMap runAttributes);

    void close();

    const char *getFileName();

    void recordScalar(const char *componentPath, const char *name, double value,
                      StringMap attributes);

    void recordStatistic(const char *componentPath, const char *name, IStatisticalSummary *statistic, StringMap attributes);

    void flush();

  protected:

    void open();

    void flushAndCheck();

    void writeField(const char *name, double value);
};

#endif
