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

#ifndef _MERGERNODES_H_
#define _MERGERNODES_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN


/**
 * Processing node which merges several input streams into one.
 */
class SCAVE_API MergerNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port out;
        PortVector ports;
    public:
        MergerNode() : out(this) {}
        virtual ~MergerNode() {}

        Port *addPort();

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;
};

class SCAVE_API MergerNodeType : public NodeType
{
    public:
        virtual const char *getName() const {return "merger";}
        virtual const char *getCategory() const {return "merger";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

/**
 * Processing node which aggregates several input streams into one.
 *
 * It can compute the average/count/minimum/maximum of its inputs.
 *
 * TODO: compute all statistics at once.
 */
class SCAVE_API AggregatorNode : public Node
{
    private:
        enum Function
        {
            Sum, Average, Count, Minimum, Maximum
        };

        Function fn;
        double acc;
        int count;

        void init();
        void collect(double value);
        double result();
    public:
        typedef std::vector<Port> PortVector;
        Port out;
        PortVector ports;
    public:
        AggregatorNode(const std::string &function);
        virtual ~AggregatorNode() {}

        Port *addPort();

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;
};

class SCAVE_API AggregatorNodeType : public NodeType
{
    public:
        virtual const char *getName() const {return "aggregator";}
        virtual const char *getCategory() const {return "merger";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual void validateAttrValues(const StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

NAMESPACE_END


#endif



