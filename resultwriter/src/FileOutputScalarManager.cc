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

#include <string>
#include <stdio.h>
#include "FileOutputScalarManager.h"
#include "OutputFileManager.h"
#include "ResultRecordingException.h"

using namespace std;

FileOutputScalarManager::FileOutputScalarManager()
{
}

FileOutputScalarManager::~FileOutputScalarManager()
{
}

FileOutputScalarManager::FileOutputScalarManager(string fileName)
{
    //FIXME TODO remove file if exists
    //if (unlink(fileName.c_str())!=0
    this->fileName = fileName;
    out = new ofstream(fileName.c_str());


    if (!out->is_open()){
        throw ResultRecordingException("Cannot create output file");
    }
    out->close();

}

void FileOutputScalarManager::open(string runID, stringMap runAttributes)
{
    this->runID = runID;
    this->runAttributes = runAttributes;
}

void FileOutputScalarManager::open()
{
    try
    {
        out->open(this->getFileName().c_str());
    }
    catch(exception e)
    {
        throw ResultRecordingException("Cannot open output scalar file ");
    }

    *out << "version " << FILE_VERSION << endl << endl;

    writeRunHeader(out, runID, runAttributes);
    flushAndCheck();
}

void FileOutputScalarManager::close()
{
    if (out->is_open())
    {
        flushAndCheck();
        out->close();
    }
}

void FileOutputScalarManager::flush()
{
    if (out->is_open())
        flushAndCheck();

}

void FileOutputScalarManager::flushAndCheck()
{
    out->flush();
    if (!out->good())
        throw ResultRecordingException("Cannot write output scalar file ");
}

string FileOutputScalarManager::getFileName()
{
    return fileName;
}

void FileOutputScalarManager::recordScalar(string componentPath, string name, double value,
                                           stringMap attributes)
{
    if (!out->is_open())
        open();
    *out << "scalar " << q(componentPath) << " " << q(name) << " " << value << endl;
    writeAttributes(out, &attributes);
}

void FileOutputScalarManager::recordStatistic(string componentPath, string name, IStatisticalSummary* statistic, stringMap attributes)
{
    if (!out)
        open();

    *out << "statistic " << q(componentPath) << " " << q(name) << endl;
    writeField("count", statistic->getN());
    writeField("mean", statistic->getMean());
    writeField("stddev", statistic->getStandardDeviation());
    writeField("sum", statistic->getSum());
    writeField("sqrsum", statistic->getSqrSum());
    writeField("min", statistic->getMin());
    writeField("max", statistic->getMax());

    writeAttributes(out, &attributes);

    if (dynamic_cast<IStatisticalSummary2 *>(statistic))
    {
        IStatisticalSummary2 *statistic2 = static_cast<IStatisticalSummary2 *>(statistic);

        writeField("weights", statistic2->getWeights());
        writeField("weightedSum", statistic2->getWeightedSum());
        writeField("sqrSumWeights", statistic2->getSqrSumWeights());
        writeField("weightedSqrSum", statistic2->getWeightedSqrSum());
    }

    if (dynamic_cast<IHistogramSummary *>(statistic))
    {
        IHistogramSummary *statistic2 = static_cast<IHistogramSummary *>(statistic);

        int n = statistic2->getNumCells();
        if (n > 0)
        {
            *out << "bin -INF " << statistic2->getUnderflowCell() << endl;
            for (int i = 0; i < n; i++)
                *out << "bin " << statistic2->getCellBoundary(i) << " " << statistic2->getCellValue(i) << endl;
            *out << "bin " << statistic2->getCellBoundary(n) << statistic2->getOverflowCell() << endl;
        }
    }
}

void FileOutputScalarManager::writeField(string name, double value)
{
    if (isNaN(value))
        *out << "field " << q(name) << " " << value << endl;
}
