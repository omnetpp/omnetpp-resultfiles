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

#ifndef _COMMONNODES_H_
#define _COMMONNODES_H_

#include <string>
#include "node.h"
#include "nodetype.h"
#include "filereader.h"

NAMESPACE_BEGIN

#ifdef _MSC_VER
#pragma warning(disable:4355) // 'this' : used in base member initializer list
#endif


/**
 * Base class for one-port producer nodes.
 */
class SCAVE_API SingleSourceNode : public Node
{
    public:
        Port out;
    public:
        SingleSourceNode() : out(this) {}
};


/**
 * Base class for one-port consumer nodes.
 */
class SCAVE_API SingleSinkNode : public Node
{
    public:
        Port in;
    public:
        SingleSinkNode() : in(this) {}
};


/**
 * Base class for processing nodes with one input and one output port.
 */
class SCAVE_API FilterNode : public Node
{
    protected:
        virtual bool isFinished() const;
    public:
        Port in;
        Port out;
    public:
        FilterNode() : in(this), out(this) {}
};

/**
 * Base class for reading input from file.
 */
class SCAVE_API ReaderNode : public Node
{
    protected:
        std::string filename;
        FileReader reader;
    public:
        ReaderNode(const char* filename, size_t bufferSize)
            : filename(filename), reader(filename, bufferSize) {}
        int64 getFileSize() { return reader.getFileSize(); }
        int64 getNumReadBytes() { return reader.getNumReadBytes(); }
};

/**
 * NodeType for SingleSourceNode.
 */
class SCAVE_API SingleSourceNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "single sources";}
        virtual bool isHidden() const {return true;}
        virtual Port *getPort(Node *node, const char *portname) const;
};


/**
 * NodeType for SingleSinkNode.
 */
class SCAVE_API SingleSinkNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "single sinks";}
        virtual bool isHidden() const {return true;}
        virtual Port *getPort(Node *node, const char *portname) const;
};


/**
 * NodeType for FilterNode.
 */
class SCAVE_API FilterNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "filter";}
        virtual bool isHidden() const {return false;}
        virtual Port *getPort(Node *node, const char *portname) const;
};

/**
 * NodeType for ReaderNode.
 */
class SCAVE_API ReaderNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "reader-node";}
        virtual bool isHidden() const {return true;}
};

NAMESPACE_END


#endif


