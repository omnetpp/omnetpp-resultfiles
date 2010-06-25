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

#include <stdio.h>
#include <locale.h>
#include "channel.h"
#include "filewriter.h"

#ifdef CHECK
#undef CHECK
#endif

USING_NAMESPACE
#define CHECK(fprintf)    if (fprintf<0) throw opp_runtime_error("Cannot write output vector file `%s'", fileName.c_str())


FileWriterNode::FileWriterNode(const char *filename, const char *banner)
{
    f = NULL;
    this->fileName = filename;
    this->banner = (banner ? banner : "");
    this->prec = DEFAULT_PRECISION;
}

FileWriterNode::~FileWriterNode()
{
}

bool FileWriterNode::isReady() const
{
    return in()->length()>0;
}

void FileWriterNode::process()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fileName.c_str(), "w");
        if (!f)
            throw opp_runtime_error("cannot open `%s' for write", fileName.c_str());

        setlocale(LC_NUMERIC, "C");

        // print file header
        CHECK(fprintf(f,"%s\n\n", banner.c_str()));
    }

    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        CHECK(fprintf(f,"%.*g\t%.*g\n", prec, a.x, prec, a.y));
    }

    if (in()->isClosing())
        fclose(f);
}

bool FileWriterNode::isFinished() const
{
    return in()->eof();
}

//--------

const char *FileWriterNodeType::getDescription() const
{
    return "Writes the output into a two-column text file.";
}

void FileWriterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output file";
}

Node *FileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();

    Node *node = new FileWriterNode(fileName);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

