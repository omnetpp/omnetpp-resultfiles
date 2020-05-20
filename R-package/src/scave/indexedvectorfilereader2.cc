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

#include "platmisc.h"
#include "opp_ctype.h"
#include "channel.h"
#include "scaveutils.h"
#include "vectorfilereader.h"
#include "indexedvectorfilereader2.h"

USING_NAMESPACE

using namespace std;

#define LL  INT64_PRINTF_FORMAT

IndexedVectorFileReaderNode2::IndexedVectorFileReaderNode2(const char *filename, size_t bufferSize) :
  ReaderNode(filename, bufferSize), index(NULL), fFinished(false)
{

}

IndexedVectorFileReaderNode2::~IndexedVectorFileReaderNode2()
{
    if (index) {
        delete index;
        index = NULL;
    }
}

Port *IndexedVectorFileReaderNode2::addVector(const VectorResult &vector)
{
    PortData& portdata = ports[vector.vectorId];
    portdata.ports.push_back(Port(this));
    Port& port = portdata.ports.back();
    return &port;
}

bool IndexedVectorFileReaderNode2::isReady() const
{
    return true;
}

void IndexedVectorFileReaderNode2::process()
{
    if (!index)
        readIndexFile();

    // read one block from each vector

    // FIXME read the blocks sequentially instead of round-robin
    bool found = false;
    for (PortMap::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        PortData &portData = it->second;
        found |= readNextBlock(portData);
    }


    if (!found)
        fFinished = true;
}

bool IndexedVectorFileReaderNode2::isFinished() const
{
    return fFinished;
}

void IndexedVectorFileReaderNode2::readIndexFile()
{
    const char *fn = filename.c_str();

    if (!IndexFile::isVectorFile(fn))
        throw opp_runtime_error("indexed vector file reader: not a vector file, file %s", fn);
    if (!IndexFile::isIndexFileUpToDate(fn))
        throw opp_runtime_error("indexed vector file reader: index file is not up to date, file %s", fn);

    string indexFileName = IndexFile::getIndexFileName(fn);
    IndexFileReader reader(indexFileName.c_str());
    index = reader.readAll();

    for (PortMap::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        int vectorId = it->first;
        PortData &portData = it->second;

        portData.vector = index->getVectorById(vectorId);

        if (!portData.vector)
            throw opp_runtime_error("indexed vector file reader: vector %d not found, file %s",
                                        vectorId, indexFileName.c_str());
    }
}

bool IndexedVectorFileReaderNode2::readNextBlock(PortData &portData)
{
    assert(portData.vector);

    VectorData *vector = portData.vector;
    if (portData.currentBlockIndex >= (int)vector->blocks.size())
        return false;

    const char *file = filename.c_str();
    file_offset_t offset;
#define CHECK(cond, msg) {if (!(cond)) throw opp_runtime_error(msg ", file %s, offset %"LL"d", file, (int64)offset); }


    Block &block = vector->blocks[portData.currentBlockIndex++];
    file_offset_t startOffset = block.startOffset;
    long count = block.getCount();

    reader.seekTo(startOffset);

    char *line;
    for (long k = 0; k < count && (line=reader.getNextLineBufferPointer())!=NULL; ++k)
    {
        offset = reader.getCurrentLineStartOffset();
        int length = reader.getCurrentLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();
        int vectorId;

        // check vector id
        CHECK((numtokens >= 3) && opp_isdigit(vec[0][0]), "vector file reader: data line too short");
        CHECK(parseInt(vec[0], vectorId), "invalid vector file syntax: invalid vector id column");
        CHECK(vectorId == vector->vectorId, "vector file reader: unexpected vector id");

        // parse columns
        Datum a = parseColumns(vec, numtokens, vector->columns, file, -1, offset);

        // write to port(s)
        for (PortVector::const_iterator port = portData.ports.begin(); port != portData.ports.end(); ++port)
            port->getChannel()->write(&a,1);
    }

    return true;
}

//-----

const char *IndexedVectorFileReaderNode2Type::getDescription() const
{
    return "Reads indexed output vector files.";
}

void IndexedVectorFileReaderNode2Type::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *IndexedVectorFileReaderNode2Type::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new IndexedVectorFileReaderNode2(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *IndexedVectorFileReaderNode2Type::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    IndexedVectorFileReaderNode2 *node1 = dynamic_cast<IndexedVectorFileReaderNode2 *>(node);
    VectorResult vector;
    if (!parseInt(portname, vector.vectorId))
        throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
    return node1->addVector(vector);
}

