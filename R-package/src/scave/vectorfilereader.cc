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

#include <string.h>
#include <stdlib.h>
#include "opp_ctype.h"
#include "channel.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "vectorfilereader.h"

NAMESPACE_BEGIN

using namespace std;


VectorFileReaderNode::VectorFileReaderNode(const char *fileName, size_t bufferSize) :
  ReaderNode(fileName, bufferSize), fFinished(false)
{
}

VectorFileReaderNode::~VectorFileReaderNode()
{
}

Port *VectorFileReaderNode::addVector(const VectorResult &vector)
{
    PortVector& portvec = ports[vector.vectorId];
    portvec.push_back(Port(this));
    Port& port = portvec.back();
    return &port;
}

bool VectorFileReaderNode::isReady() const
{
    return true;
}

/**
 * Parses columns of one line in the vector file.
 */
Datum parseColumns(char **tokens, int numtokens, const string &columns, const char *file, int64 lineno, file_offset_t offset)
{
    Datum a;
    int colno = columns.size();

    if (colno > numtokens - 1)
        throw ResultFileFormatException("invalid vector file syntax: missing columns", file, lineno, offset);
    if (numtokens - 1 > colno)
        throw ResultFileFormatException("invalid vector file syntax: extra columns", file, lineno, offset);

    // optimization:
    //   first process the two most common case, then the general case

    if (colno == 2 && columns[0] == 'T' && columns[1] == 'V')
    {
        // parse time and value
        if (!parseSimtime(tokens[1],a.xp) || !parseDouble(tokens[2],a.y))
            throw ResultFileFormatException("invalid vector file syntax: invalid time or value column", file, lineno, offset);
        a.eventNumber = -1;
        a.x = a.xp.dbl();
    }
    else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V')
    {
        // parse event number, time and value
        if (!parseInt64(tokens[1], a.eventNumber) || !parseSimtime(tokens[2],a.xp) || !parseDouble(tokens[3],a.y))
            throw ResultFileFormatException("invalid vector file syntax: invalid event number, time or value column", file, lineno, offset);
        a.x = a.xp.dbl();
    }
    else // interpret general case
    {
        a.eventNumber = -1;
        for (int i = 0; i < (int)columns.size(); ++i)
        {
            switch (columns[i])
            {
            case 'E':
                if (!parseInt64(tokens[i+1], a.eventNumber))
                    throw ResultFileFormatException("invalid vector file syntax: invalid event number", file, lineno, offset);
                break;
            case 'T':
                if (!parseSimtime(tokens[i+1], a.xp))
                    throw ResultFileFormatException("invalid vector file syntax: invalid time", file, lineno, offset);
                a.x = a.xp.dbl();
                break;
            case 'V':
                if (!parseDouble(tokens[i+1], a.y))
                    throw ResultFileFormatException("invalid vector file syntax: invalid value", file, lineno, offset);
                break;
            default:
                throw ResultFileFormatException("invalid vector file syntax: unknown column type", file, lineno, offset);
            }
        }
    }

    return a;
}

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) { throw ResultFileFormatException("vector file reader" msg, file, lineNo); }

void VectorFileReaderNode::process()
{
    const char *file = filename.c_str();
    char *line;
    for (int k=0; k<1000 && (line=reader.getNextLineBufferPointer())!=NULL; k++)
    {
        int64 lineNo = reader.getNumReadLines();
        int length = reader.getCurrentLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();

        if (vec[0][0] == 'v' && strcmp(vec[0], "vector") == 0)
        {
            CHECK(numtokens >= 4, "broken vector declaration");

            int vectorId;
            CHECK(parseInt(vec[1], vectorId), "malformed vector in vector declaration");
            if (ports.find(vectorId) != ports.end())
                columns[vectorId] = (numtokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);
        }
        else if (vec[0][0] == 'v' && strcmp(vec[0], "version") == 0)
        {
            int version;
            CHECK(numtokens >= 2, "missing version number");
            CHECK(parseInt(vec[1], version), "version is not a number");
            CHECK(version <= 2, "expects version 2 or lower");
        }
        else if (numtokens>=3 && opp_isdigit(vec[0][0]))  // silently ignore incomplete lines
        {
            // extract vector id
            int vectorId;
            CHECK(parseInt(vec[0], vectorId), "invalid vector id column");

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec!=ports.end())
            {
                ColumnMap::iterator columnSpec = columns.find(vectorId);
                CHECK(columnSpec != columns.end(), "missing vector declaration");

                // parse columns
                Datum a = parseColumns(vec, numtokens, columnSpec->second, file, lineNo, -1);

                // write to port(s)
                for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                    p->getChannel()->write(&a,1);

                //DBG(("vectorfilereader: written id=%d (%"LL"d,%g,%g)\n", vectorId, a.eventNumber, a.x, a.y));
            }
        }
    }

    if (line == NULL) {
        fFinished = true;
    }
}

bool VectorFileReaderNode::isFinished() const
{
    return fFinished;
}

//-----

const char *VectorFileReaderNodeType::getDescription() const
{
    return "Reads output vector files.";
}

void VectorFileReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *VectorFileReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new VectorFileReaderNode(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *VectorFileReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    VectorFileReaderNode *node1 = dynamic_cast<VectorFileReaderNode *>(node);
    VectorResult vector;
    vector.vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vector);
}

NAMESPACE_END


