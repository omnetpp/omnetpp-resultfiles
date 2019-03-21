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
#include "platmisc.h"
#include "opp_ctype.h"
#include "channel.h"
#include "scaveutils.h"
#include "vectorfilereader.h"
#include "indexedvectorfilereader.h"

USING_NAMESPACE

using namespace std;

#define LL  INT64_PRINTF_FORMAT

IndexedVectorFileReaderNode::IndexedVectorFileReaderNode(const char *filename, size_t bufferSize) :
  ReaderNode(filename, bufferSize), index(NULL), currentBlockIndex(0)
{

}

IndexedVectorFileReaderNode::~IndexedVectorFileReaderNode()
{
    if (index) {
        delete index;
        index = NULL;
    }
}

Port *IndexedVectorFileReaderNode::addVector(const VectorResult &vector)
{
    PortData& portdata = ports[vector.vectorId];
    portdata.ports.push_back(Port(this));
    Port& port = portdata.ports.back();
    return &port;
}

bool IndexedVectorFileReaderNode::isReady() const
{
    return true;
}

void IndexedVectorFileReaderNode::process()
{
    if (!index)
        readIndexFile();

    long bytesRead = 0;
    while (currentBlockIndex < blocksToRead.size() && bytesRead < 64 * 1024)
    {
        BlockAndPortData &blockAndPort = blocksToRead[currentBlockIndex++];
        bytesRead += readBlock(blockAndPort.blockPtr, blockAndPort.portDataPtr);
    }
}

bool IndexedVectorFileReaderNode::isFinished() const
{
    return index && currentBlockIndex >= blocksToRead.size();
}

void IndexedVectorFileReaderNode::readIndexFile()
{
    const char *fn = filename.c_str();

    if (!IndexFile::isVectorFile(fn))
        throw opp_runtime_error("indexed vector file reader: not a vector file, file %s", fn);
    if (!IndexFile::isIndexFileUpToDate(fn))
        throw opp_runtime_error("indexed vector file reader: index file is not up to date, file %s", fn);

    string indexFileName = IndexFile::getIndexFileName(fn);
    IndexFileReader reader(indexFileName.c_str());
    index = reader.readAll();

    for (VectorIdToPortMap::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        int vectorId = it->first;
        PortData &portData = it->second;

        portData.vector = index->getVectorById(vectorId);

        if (!portData.vector)
            throw opp_runtime_error("indexed vector file reader: vector %d not found, file %s",
                                        vectorId, indexFileName.c_str());

        Blocks &blocks = portData.vector->blocks;
        for (Blocks::iterator it = blocks.begin(); it != blocks.end(); ++it)
            blocksToRead.push_back(BlockAndPortData(&(*it), &portData));
    }

    sort(blocksToRead.begin(), blocksToRead.end());
}


long IndexedVectorFileReaderNode::readBlock(const Block *blockPtr, const PortData *portDataPtr)
{
    assert(blockPtr);
    assert(portDataPtr->vector);

    const char *file = filename.c_str();
    file_offset_t offset;
#define CHECK(cond, msg) {if (!(cond)) throw opp_runtime_error(msg ", file %s, offset %"LL"d", file, (int64)offset); }

    VectorData *vector = portDataPtr->vector;
    file_offset_t startOffset = blockPtr->startOffset;
    long count = blockPtr->getCount();

    reader.seekTo(startOffset);

    long readTime=0;
    long tokenizeTime = 0;
    long parseTime = 0;
//  printf("readBlock ");

    char *line;
    for (long k = 0; k < count /*&& (line=reader.getNextLineBufferPointer())!=NULL*/; ++k)
    {
        TIME(readTime,line=reader.getNextLineBufferPointer());

        if (!line)
            break;


        offset = reader.getCurrentLineStartOffset();
        int length = reader.getCurrentLineLength();
        TIME(tokenizeTime, tokenizer.tokenize(line, length));

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();
        int vectorId;

        // check vector id
        CHECK((numtokens >= 3) && opp_isdigit(vec[0][0]), "vector file reader: data line too short");
        CHECK(parseInt(vec[0], vectorId), "invalid vector file syntax: invalid vector id column");
        CHECK(vectorId == vector->vectorId, "vector file reader: unexpected vector id");

        // parse columns
        Datum a;
        TIME(parseTime, a = parseColumns(vec, numtokens, vector->columns, file, -1, offset));

        // write to port(s)
        for (PortVector::const_iterator port = portDataPtr->ports.begin(); port != portDataPtr->ports.end(); ++port)
            port->getChannel()->write(&a,1);
    }

//    printf("read: %ldms tokenize: %ldms parse: %ldms\n",
//          readTime/1000, tokenizeTime/1000, parseTime/1000);

    return blockPtr->size;
}

//-----

const char *IndexedVectorFileReaderNodeType::getDescription() const
{
    return "Reads indexed output vector files.";
}

void IndexedVectorFileReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *IndexedVectorFileReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new IndexedVectorFileReaderNode(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *IndexedVectorFileReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    IndexedVectorFileReaderNode *node1 = dynamic_cast<IndexedVectorFileReaderNode *>(node);
    VectorResult vector;
    if (!parseInt(portname, vector.vectorId))
        throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
    return node1->addVector(vector);
}

