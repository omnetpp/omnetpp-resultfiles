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

#include <algorithm>
#include <locale.h>
#include <sys/stat.h>
#include "intxtypes.h"
#include "exception.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "stringutil.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "indexfile.h"

#define LL INT64_PRINTF_FORMAT

USING_NAMESPACE

static bool serialLess(const Block &first, const Block &second)
{
    return first.endSerial() < second.endSerial();
}

const Block* VectorData::getBlockBySerial(long serial) const
{
    if (serial < 0 || serial >= getCount())
        return NULL;

    Block blockToFind;
    blockToFind.startSerial = serial;
    Blocks::const_iterator first = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, serialLess);
    assert(first == blocks.end() || first->endSerial() > serial); // first block ending after serial

    if (first != blocks.end()) {
        assert(first->contains(serial));
        return &(*first);
    }
    else
        return NULL;
}

// ordering of blocks
static bool simtimeLess(const Block &first, const Block &second)
{
    return first.endTime < second.startTime;
}

// ordering of reversed blocks
static bool simtimeGreater(const Block &first, const Block &second)
{
    return first.startTime > second.endTime;
}


const Block *VectorData::getBlockBySimtime(simultime_t simtime, bool after) const
{
    Block blockToFind;
    blockToFind.startTime = simtime;
    blockToFind.endTime = simtime;

    if (after)
    {
        Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);
        return first != blocks.end() ? &(*first) : NULL;
    }
    else
    {
        Blocks::const_reverse_iterator last = std::lower_bound(blocks.rbegin(), blocks.rend(), blockToFind, simtimeGreater);
        return last != blocks.rend() ? &(*last) : NULL;
    }
}

Blocks::size_type VectorData::getBlocksInSimtimeInterval(simultime_t startTime, simultime_t endTime, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const
{
    Block blockToFind;
    blockToFind.startTime = startTime;
    blockToFind.endTime = endTime;

    Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);
    Blocks::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);

    assert(first == blocks.end() || first->endTime >= startTime);
    assert(last == blocks.end() || last->startTime > endTime);
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

// ordering of blocks
static bool eventnumLess(const Block &first, const Block &second)
{
    return first.endEventNum < second.startEventNum;
}

// ordering of reversed blocks
static bool eventnumGreater(const Block &first, const Block &second)
{
    return first.startEventNum > second.endEventNum;
}

const Block *VectorData::getBlockByEventnum(eventnumber_t eventNum, bool after) const
{
    Block blockToFind;
    blockToFind.startEventNum = eventNum;
    blockToFind.endEventNum = eventNum;

    if (after)
    {
        Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
        return first != blocks.end() ? &(*first) : NULL;
    }
    else
    {
        Blocks::const_reverse_iterator last = std::lower_bound(blocks.rbegin(), blocks.rend(), blockToFind, eventnumGreater);
        return last != blocks.rend() ? &(*last) : NULL;
    }
}

Blocks::size_type VectorData::getBlocksInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const
{
    Block blockToFind;
    blockToFind.startEventNum = startEventNum;
    blockToFind.endEventNum = endEventNum;

    Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
    Blocks::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);

    assert(first == blocks.end() || first->endEventNum >= startEventNum);
    assert(last == blocks.end() || last->startEventNum > endEventNum);
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) throw ResultFileFormatException(msg, filename, lineNo);

bool RunData::parseLine(char **tokens, int numTokens, const char *filename, int64 lineNo)
{
    if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0)
    {
        CHECK(numTokens >= 3, "'attr <name> <value>' expected");
        this->attributes[tokens[1]] = tokens[2];
        // the "runNumber" attribute is also stored separately
        if (strcmp(tokens[1], "runNumber") == 0) {
            CHECK(parseInt(tokens[2], this->runNumber), "runNumber: an integer expected");
        }
        return true;
    }
    else if (tokens[0][0] == 'p' && strcmp(tokens[0], "param") == 0)
    {
        CHECK(numTokens >= 3, "'param <namePattern> <value>' expected");
        this->moduleParams[tokens[1]] = tokens[2];
        return true;
    }
    else if (tokens[0][0] == 'r' && strcmp(tokens[0], "run") == 0)
    {
        CHECK(numTokens >= 2, "missing run name");
        this->runName = tokens[1];
        return true;
    }

    return false;
}

#undef CHECK
#define CHECK(fprintf)    if (fprintf<0) throw opp_runtime_error("Cannot write output file `%s'", filename)

void RunData::writeToFile(FILE *file, const char *filename) const
{
    if (runName.size() > 0)
    {
        CHECK(fprintf(file, "run %s\n", runName.c_str()));
    }
    for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
    {
        CHECK(fprintf(file, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    for (StringMap::const_iterator it = moduleParams.begin(); it != moduleParams.end(); ++it)
    {
        CHECK(fprintf(file, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
}
//=========================================================================
static bool isFileReadable(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f != NULL)
        fclose(f);
    return f != NULL;
}

bool IndexFile::isIndexFile(const char *filename)
{
    int len = strlen(filename);
    return (len >= 4) && (strcmp(filename+len-4, ".vci") == 0);
}

bool IndexFile::isVectorFile(const char *filename)
{
    // XXX check contents?
    int len = strlen(filename);
    return (len >= 4) && (strcmp(filename+len-4, ".vec") == 0);
}

std::string IndexFile::getVectorFileName(const char *filename)
{
    std::string vectorFileName(filename);
    std::string::size_type pos = vectorFileName.rfind('.');
    if (pos != std::string::npos)
        vectorFileName.replace(vectorFileName.begin()+pos, vectorFileName.end(), ".vec");
    else
        vectorFileName.append(".vec");
    return vectorFileName;
}

std::string IndexFile::getIndexFileName(const char *filename)
{
    std::string indexFileName(filename);
    std::string::size_type pos = indexFileName.rfind('.');
    if (pos != std::string::npos)
        indexFileName.replace(indexFileName.begin()+pos, indexFileName.end(), ".vci");
    else
        indexFileName.append(".vci");
    return indexFileName;
}

bool IndexFile::isIndexFileUpToDate(const char *filename)
{
    std::string indexFileName, vectorFileName;

    if (isIndexFile(filename))
    {
        indexFileName = std::string(filename);
        vectorFileName = getVectorFileName(filename);
    }
    else
    {
        indexFileName = getIndexFileName(filename);
        vectorFileName = std::string(filename);
    }

    if (!isFileReadable(indexFileName.c_str()))
        return false;

    IndexFileReader reader(indexFileName.c_str());
    VectorFileIndex *index = reader.readFingerprint();

    // when the fingerprint not found assume the index file is being written therefore it is up to date
    if (!index)
        return true;

    bool uptodate = index->fingerprint.check(vectorFileName.c_str());
    delete index;
    return uptodate;
}

//=========================================================================
FingerPrint::FingerPrint(const char *vectorFileName)
{
    struct opp_stat_t s;
    if (opp_stat(vectorFileName, &s) != 0)
        throw opp_runtime_error("vector file '%s' does not exist", vectorFileName);

    this->lastModified = (int64)s.st_mtime;
    this->fileSize = (int64)s.st_size;
}

bool FingerPrint::check(const char *vectorFileName)
{
    struct opp_stat_t s;
    if (opp_stat(vectorFileName, &s) == 0)
    {
        return (this->lastModified >= (int64)s.st_mtime) && (this->fileSize == (int64)s.st_size);
    }
    return false;
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg,line) if (!(cond)) throw ResultFileFormatException(msg, filename.c_str(), line);

// see ifilemgr.h
#define MIN_BUFFER_SIZE 512

IndexFileReader::IndexFileReader(const char *filename)
    : filename(filename)
{
}

VectorFileIndex *IndexFileReader::readAll()
{
    // read vector and blocks
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = new VectorFileIndex();
    while ((line=reader.getNextLineBufferPointer())!=NULL)
    {
        int64 lineNum = reader.getNumReadLines();
        int len=reader.getCurrentLineLength();
        numTokens=tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();
        parseLine(tokens, numTokens, index, lineNum);
    }
    return index;
}

VectorFileIndex *IndexFileReader::readFingerprint()
{
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = NULL;
    while ((line=reader.getNextLineBufferPointer())!=NULL)
    {
        int64 lineNum = reader.getNumReadLines();
        int len = reader.getCurrentLineLength();
        numTokens = tokenizer.tokenize(line,len);
        tokens = tokenizer.tokens();

        if (numTokens == 0 || tokens[0][0] == '#')
            continue;
        else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0)
        {
            index = new VectorFileIndex();
            parseLine(tokens, numTokens, index, lineNum);
        }
        else
            break;
    }

    // missing fingerprint: possible if the writing of the index file is in progress
    // CHECK(index, "missing fingerprint", -1);

    return index;
}


void IndexFileReader::parseLine(char **tokens, int numTokens, VectorFileIndex *index, int64 lineNum)
{
    if (numTokens == 0 || tokens[0][0] == '#')
        return;

    long count;
    double min;
    double max;
    double sum;
    double sumSqr;

    if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0)
    {
        CHECK(numTokens >= 5, "invalid vector declaration", lineNum);

        VectorData vector;
        CHECK(parseInt(tokens[1], vector.vectorId), "invalid vector id", lineNum);
        vector.moduleName = tokens[2];
        vector.name = tokens[3];
        vector.columns = tokens[4];

        index->addVector(vector);
    }
    else if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0 && index->getNumberOfVectors() > 0) // vector attr
    {
        CHECK(numTokens == 3, "malformed vector attribute", lineNum);
        VectorData *lastVector = index->getVectorAt(index->getNumberOfVectors()-1);
        lastVector->attributes[tokens[1]] = tokens[2];
    }
    else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0)
    {
        int64 fileSize;
        int64 lastModified;
        CHECK(numTokens >= 3, "missing file attributes", lineNum);
        CHECK(parseInt64(tokens[1], fileSize), "file size is not a number", lineNum);
        CHECK(parseInt64(tokens[2], lastModified), "modification date is not a number", lineNum);
        index->fingerprint.fileSize = fileSize;
        index->fingerprint.lastModified = lastModified;
    }
    else if (tokens[0][0] == 'v' && strcmp(tokens[0], "version") == 0)
    {
        int version;
        CHECK(numTokens >= 2, "missing version number", lineNum);
        CHECK(parseInt(tokens[1], version), "version is not a number", lineNum);
        CHECK(version <= 2, "expects version 2 or lower", lineNum);
    }
    else if (tokens[0][0] == 'i' && strcmp(tokens[0], "itervar") == 0) {
        return;
    }
    else if (index->run.parseLine(tokens, numTokens, filename.c_str(), lineNum))
    {
        return;
    }
    else // blocks
    {
        CHECK(numTokens >= 10, "missing fields from block", lineNum);

        int id;
        CHECK(parseInt(tokens[0], id), "malformed vector id", lineNum);
        VectorData *vector = index->getVectorById(id);
        CHECK(vector, "missing vector definition", lineNum);

        Block block;
        block.startSerial = vector->blocks.size() > 0 ? vector->blocks.back().endSerial() : 0;
        int i = 1; // column index
        CHECK(parseInt64(tokens[i++], block.startOffset), "invalid file offset", lineNum);
        CHECK(parseInt64(tokens[i++], block.size), "invalid block size", lineNum);
        if (vector->hasColumn('E'))
        {
            CHECK(parseInt64(tokens[i++], block.startEventNum) && parseInt64(tokens[i++], block.endEventNum),
                "invalid event numbers", lineNum);
        }
        if (vector->hasColumn('T'))
        {
            CHECK(parseSimtime(tokens[i++], block.startTime) && parseSimtime(tokens[i++], block.endTime),
                "invalid simulation time", lineNum);
        }
        if (vector->hasColumn('V'))
        {
            CHECK(parseLong(tokens[i++], count) && parseDouble(tokens[i++], min) && parseDouble(tokens[i++], max) &&
                    parseDouble(tokens[i++], sum) && parseDouble(tokens[i++], sumSqr), "invalid statistics data", lineNum);
            block.stat = Statistics(count, min, max, sum, sumSqr);
        }
        vector->addBlock(block);
    }
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw opp_runtime_error("Cannot write output file `%s'", filename.c_str())
#define INDEX_FILE_VERSION 2

IndexFileWriter::IndexFileWriter(const char *filename, int precision)
    : filename(filename), precision(precision), file(NULL)
{
}

IndexFileWriter::~IndexFileWriter()
{
    if (file != NULL)
    {
        closeFile();
    }
}

void IndexFileWriter::writeAll(const VectorFileIndex& index)
{
    openFile();
    writeFingerprint(index.vectorFileName);
    writeRun(index.run);

    int numOfVectors = index.getNumberOfVectors();
    for (int i = 0; i < numOfVectors; ++i)
    {
        const VectorData *vectorRef = index.getVectorAt(i);
        writeVector(*vectorRef);
    }

    closeFile();
}

void IndexFileWriter::writeFingerprint(std::string vectorFileName)
{
    FingerPrint fingerprint(vectorFileName.c_str());

    if (file == NULL)
        openFile();

    file_offset_t saveOffset = opp_ftell(file);
    opp_fseek(file, 0, SEEK_SET);
    CHECK(fprintf(file, "file %"LL"d %"LL"d", fingerprint.fileSize, fingerprint.lastModified));
    opp_fseek(file, saveOffset, SEEK_SET);
}

void IndexFileWriter::writeRun(const RunData &run)
{
    if (file == NULL)
        openFile();
    run.writeToFile(file, filename.c_str());
}

void IndexFileWriter::writeVector(const VectorData &vector)
{
    if (file == NULL)
        openFile();

    int numBlocks = vector.blocks.size();
    if (numBlocks > 0)
    {
        writeVectorDeclaration(vector);
        writeVectorAttributes(vector);

        for (int i=0; i<numBlocks; i++)
        {
            writeBlock(vector, vector.blocks[i]);
        }
    }
}

void IndexFileWriter::writeVectorDeclaration(const VectorData &vector)
{
    CHECK(fprintf(file, "vector %d  %s  %s  %s\n",
          vector.vectorId, QUOTE(vector.moduleName.c_str()), QUOTE(vector.name.c_str()), vector.columns.c_str()));

}

void IndexFileWriter::writeVectorAttributes(const VectorData &vector)
{
    for (StringMap::const_iterator it=vector.attributes.begin(); it != vector.attributes.end(); ++it)
    {
        CHECK(fprintf(file, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
    }
}

void IndexFileWriter::writeBlock(const VectorData &vector, const Block &block)
{
    static char buff1[64], buff2[64];
    char *e;

    if (block.getCount() > 0)
    {
        CHECK(fprintf(file, "%d\t%"LL"d %"LL"d", vector.vectorId, (int64)block.startOffset, (int64)block.size));
        if (vector.hasColumn('E')) { CHECK(fprintf(file, " %"LL"d %"LL"d", block.startEventNum, block.endEventNum)); }
        if (vector.hasColumn('T')) { CHECK(fprintf(file, " %s %s",
                                                BigDecimal::ttoa(buff1, block.startTime, e),
                                                BigDecimal::ttoa(buff2, block.endTime, e))); }
        if (vector.hasColumn('V')) { CHECK(fprintf(file, " %ld %.*g %.*g %.*g %.*g",
                                                block.getCount(), precision, block.getMin(), precision, block.getMax(),
                                                precision, block.getSum(), precision, block.getSumSqr())); }
        CHECK(fprintf(file, "\n"));
    }
}

void IndexFileWriter::openFile()
{
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
        throw opp_runtime_error("Cannot open index file: %s", filename.c_str());

    setlocale(LC_NUMERIC, "C");

    // space for header
    CHECK(fprintf(file, "%64s\n", ""));
    // version
    CHECK(fprintf(file, "version %d\n", INDEX_FILE_VERSION));
}

void IndexFileWriter::closeFile()
{
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
}

