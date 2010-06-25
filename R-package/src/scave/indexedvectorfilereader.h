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

#ifndef _INDEXEDVECTORFILEREADER_H_
#define _INDEXEDVECTORFILEREADER_H_

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "indexfile.h"
#include "resultfilemanager.h"

NAMESPACE_BEGIN

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API IndexedVectorFileReaderNode : public ReaderNode
{
    typedef std::vector<Port> PortVector;

    struct PortData
    {
        VectorData *vector;
        PortVector ports;

        PortData() : vector(NULL) {}
    };

    struct BlockAndPortData
    {
        Block *blockPtr;
        PortData *portDataPtr;

        BlockAndPortData(Block *blockPtr, PortData *portDataPtr)
            : blockPtr(blockPtr), portDataPtr(portDataPtr) {}

        bool operator<(const BlockAndPortData& other) const
        {
            return this->blockPtr->startOffset < other.blockPtr->startOffset;
        }
    };

    typedef std::map<int,PortData> VectorIdToPortMap;

    private:
        VectorIdToPortMap ports;
        VectorFileIndex *index;
        std::vector<BlockAndPortData> blocksToRead;
        unsigned int currentBlockIndex;
        LineTokenizer tokenizer;

    public:
        IndexedVectorFileReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~IndexedVectorFileReaderNode();

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;

    private:
        void readIndexFile();
        long readBlock(const Block *blockPtr, const PortData *portDataPtr);
};


class SCAVE_API IndexedVectorFileReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *getName() const {return "indexedvectorfilereader";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


NAMESPACE_END


#endif


