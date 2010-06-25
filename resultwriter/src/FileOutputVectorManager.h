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

#ifndef __FILOUTPUTVECTORMANAGER_H
#define __FILOUTPUTVECTORMANAGER_H

#include <stdlib.h>
#include <vector>
#include <exception>
#include <map>
#include <set>
#include <string>
#include "OutputFileManager.h"
#include "IOutputVectorManager.h"

class FileOutputVectorManager;

/**
 * An output vector manager that writes OMNeT++ vector (".vec") files.
 * Recording event numbers ("ETV" vectors) is not supported, because it is
 * practically only useful for sequence charts.
 *
 * This class does not support filtering (of vectors or recorded data)
 * at all -- this functionality may be added via subclasses.
 *
 * @author Andras
 */
class OutputVector : public IOutputVector
{
  public:
    int id;
    std::string header;

    int n;
    std::vector<double> times;
    std::vector<double> values;

    double blockStartTime;
    double blockEndTime;
    double min;
    double max;
    double sum;
    double sqrSum;
    friend class FileOutputVectorManager;
    FileOutputVectorManager *fileOutputVector;

  public:
     OutputVector(int id, const char *componentPath, const char *vectorName,
                  StringMap& attributes);
    ~OutputVector();

    void close();

    void flush();

    bool record(double value);

    bool record(double time, double value);

  protected:
    void writeVectorHeader() throw();

    void writeBlock();
};

class FileOutputVectorManager : public OutputFileManager, public IOutputVectorManager
{
  public:
    static const int FILE_VERSION = 2;
    friend class OutputVector;

  protected:
    std::string runID;
    StringMap runAttributes;
    std::string fileName;
    std::ofstream *out;
    std::string indexFileName;
    std::ofstream *indexOut;

    ISimulationTimeProvider *simtimeProvider;

    int perVectorLimit;
    int totalLimit;

    int lastId;
    int nbuffered;

    std::vector<OutputVector*>  vectors;

  public:
    FileOutputVectorManager();
    FileOutputVectorManager(const char *fileName);
    ~FileOutputVectorManager();

    ISimulationTimeProvider *getSimtimeProvider();

    void setSimtimeProvider(ISimulationTimeProvider *simtimeProvider);

    int getPerVectorBufferLimit();

    void setPerVectorBufferLimit(int count);

    int getTotalBufferLimit();

    void setTotalBufferLimit(int count);

    void open(const char *runID, StringMap runAttributes);

    void open();

    void close();

    void flush();

    const char *getFileName();

    IOutputVector *createVector(const char *componentPath, const char *vectorName,
                                StringMap& attributes);

  protected:
    void flushAndCheck();

    void changed(OutputVector *vector);
};

#endif

