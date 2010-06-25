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

#include <sstream>
#include <exception>
#include "FileOutputVectorManager.h"
#include "OutputFileManager.h"
#include "ResultRecordingException.h"

static double zero = 0.0;
double const NaN = zero / zero;

using namespace std;

OutputVector::OutputVector(int id, const string&componentPath, const string&vectorName, StringMap& attributes)
{
    blockStartTime = 0;
    blockEndTime = 0;
    n = 0;
    min = NaN;
    max = NaN;
    sum = 0;
    sqrSum = 0;
    this->id = id;

    // postpone writing out vector declaration until there's actually something to record
    ostringstream *outstream = new ostringstream();
    *outstream << "vector " << id << " " << OutputFileManager::q(componentPath) << " " << OutputFileManager::q(vectorName) << " TV" << endl;

    OutputFileManager::writeAttributes(outstream, &attributes);
    header = outstream->str();
}

OutputVector::~OutputVector()
{
}

void OutputVector::writeVectorHeader() throw()
{
    if (!fileOutputVector->out->is_open())
        fileOutputVector->open();
    *(fileOutputVector->out) << header;
    *(fileOutputVector->indexOut) << header;
    header.clear();
}

void OutputVector::close()
{
    fileOutputVector->flush();
    vector<OutputVector*>::iterator iter;
    for (iter = fileOutputVector->vectors.begin(); iter != fileOutputVector->vectors.end(); ++iter)
    {
        if ((*iter)->id == this->id)
        {
            fileOutputVector->vectors.erase(iter);
        }
    }

    id = -1;                    // i.e. dead object
}

void OutputVector::flush()
{
    if (id == -1)
        throw ResultRecordingException("Output vector already closed");

    writeBlock();               // implies file flushing as well
}

bool OutputVector::record(double value)
{
    if (!fileOutputVector->simtimeProvider)
        throw ResultRecordingException("Simtime provider not yet specified");

    return record(fileOutputVector->simtimeProvider->getSimulationTime(), value);
}

bool OutputVector::record(double time, double value)
{

    if (id == -1)
        throw
            ResultRecordingException("Attempt to write to an output vector that's already closed");

    if (time < blockEndTime)
        throw
            ResultRecordingException("Vector data must be recorded in increasing timestamp order");

    // store
    times.push_back(time);
    values.push_back(value);
    if (n == 0)
        blockStartTime = time;
    blockEndTime = time;
    n++;

    // update statistics
    if (min > value || isNaN(min))
        min = value;
    if (max < value || isNaN(max))
        max = value;
    sum += value;
    sqrSum += value * value;

    ++fileOutputVector->nbuffered;

    // flush if needed
    fileOutputVector->changed(this);

    return false;
}

void OutputVector::writeBlock()
{
    try
    {
        // write out vector declaration if not yet done
        if (!header.empty())
            writeVectorHeader();

        // write data
        long blockOffset = fileOutputVector->out->tellp();
        for (int i = 0; i < n; i++)
            *(fileOutputVector->
              out) << id << " " << (float) times[i] << " " << (float) values[i] << endl;

        // make sure that the offsets referred   to by the index file are exists in the vector file
        // so the index can be used to access the vector file while it is being written
        fileOutputVector->out->flush();
        if (!fileOutputVector->out->good())
            throw ResultRecordingException("Cannot write output vector file");

        long blockSize = (long) (fileOutputVector->out->tellp()) - blockOffset;

        // write index
        *(fileOutputVector->indexOut) << id << " " << blockOffset << " " << blockSize << " " <<
            blockStartTime << " " << blockEndTime << " " <<
            n << " " << min << " " << max << " " << sum << " " << sqrSum << endl;

        // reset block
        fileOutputVector->nbuffered -= n;
        n = 0;
        min = NaN;
        max = NaN;
        sum = 0;
        sqrSum = 0;
        times.clear();
        values.clear();
    }
    catch(exception e)
    {
        throw ResultRecordingException(std::string("Error recording vector results: ") + e.what());
    }
}

FileOutputVectorManager::FileOutputVectorManager()
{
}

FileOutputVectorManager::~FileOutputVectorManager()
{
    delete simtimeProvider;
    delete out;
    delete indexOut;
}

FileOutputVectorManager::FileOutputVectorManager(const string&file)
{
    perVectorLimit = 1000;
    totalLimit = 1000000;
    lastId = 0;
    nbuffered = 0;

    this->fileName = file;

    //FIXME this opens and immediately closes the file!!!
    try
    {
        out = new ofstream(fileName.c_str());
        out->exceptions(ifstream::failbit | ifstream::badbit);
    }
    catch(exception e)
    {
        throw ResultRecordingException("Cannot delete old output vector file"); //XXX
    }

    indexFileName = fileName;
    size_t pos = fileName.find_last_of("\\.[^./\\:]*$");
    indexFileName.replace(pos, indexFileName.length(), "");
    indexFileName += ".vci";
    try
    {
        indexOut = new ofstream(indexFileName.c_str(),ios::out|ios::trunc);
    }
    catch(exception e)
    {
        throw ResultRecordingException(std::string("Cannot delete old output vector index file ") + indexFileName); //XXX
    }
    out->close();
    indexOut->close();
}

ISimulationTimeProvider *FileOutputVectorManager::getSimtimeProvider()
{
    return simtimeProvider;
}

void FileOutputVectorManager::setSimtimeProvider(ISimulationTimeProvider *simtimeProvider)
{
    this->simtimeProvider = simtimeProvider;
}

int FileOutputVectorManager::getPerVectorBufferLimit()
{
    return perVectorLimit;
}

void FileOutputVectorManager::setPerVectorBufferLimit(int count)
{
    this->perVectorLimit = count;
}

int FileOutputVectorManager::getTotalBufferLimit()
{
    return totalLimit;
}

void FileOutputVectorManager::setTotalBufferLimit(int count)
{
    this->totalLimit = count;
}

void FileOutputVectorManager::open(const string&runID, StringMap runAttributes)
{
    this->runID = runID;
    this->runAttributes = runAttributes;
}

void FileOutputVectorManager::open()
{
    try
    {
        out->open(this->fileName.c_str(), ios::app);
    }
    catch(exception e)
    {
        throw ResultRecordingException("Cannot open output vector file");
    }

    try
    {
        indexOut->open(indexFileName.c_str(), ios::out|ios::trunc);
    }
    catch(exception e)
    {
        throw ResultRecordingException("Cannot open output vector index file");
    }

    *out << "version " << FILE_VERSION << endl << endl;

    writeRunHeader(out, runID, runAttributes);

    *indexOut<<"                                                                  "<<endl;
    *indexOut << "version " << FILE_VERSION << endl << endl;

    writeRunHeader(indexOut, runID, runAttributes);

    flushAndCheck();
}

void FileOutputVectorManager::close()
{

    if (out->is_open())
    {
        flush();
        out->close();

        // record size and timestamp of the vector file, for up-to-date checks
        try
        {
            indexOut->seekp(ios::beg);
        }
        catch(exception e)
        {
            throw ResultRecordingException("Cannot rewind output vector index file ");
        }

        *indexOut << "file " << /*LENGTH*/ " " << endl/*file.lastModified()/1000) */ ;  //FIXME TODO
        indexOut->flush();
        if (indexOut->bad())
            throw ResultRecordingException("Cannot write output vector index file ");
        indexOut->close();
    }

    vector<OutputVector*>::iterator iter;
    for (iter = vectors.begin(); iter != vectors.end(); ++iter)
        delete *iter;

}

void FileOutputVectorManager::flush()
{
    vector<OutputVector*>::iterator iter;
    for (iter = vectors.begin(); iter != vectors.end(); ++iter)
        (*iter)->writeBlock();

    if (out->is_open())
        flushAndCheck();
}

void FileOutputVectorManager::flushAndCheck()
{
    out->flush();
    if (out->bad())
        throw ResultRecordingException("Cannot write output vector file ");
    indexOut->flush();
    if (indexOut->bad())
        throw ResultRecordingException("Cannot write output vector index file ");
}

const string&FileOutputVectorManager::getFileName()
{
    return fileName;
}

IOutputVector *FileOutputVectorManager::createVector(const string& componentPath, const string& vectorName,
                                                     StringMap& attributes)
{
    int id = ++lastId;
    OutputVector *vector = new OutputVector(id, componentPath, vectorName, attributes);
    vector->fileOutputVector = this;
    vectors.push_back(vector);
    return vector;
}

void FileOutputVectorManager::changed(OutputVector *vect)
{
    if (vect->n > perVectorLimit)
    {
        vect->writeBlock();
    }
    else
    {
        if (nbuffered > totalLimit)
        {
            vector<OutputVector*>::iterator iter;
            for (iter = vectors.begin(); iter != vectors.end(); ++iter)
                (*iter)->writeBlock();
        }
    }
}

