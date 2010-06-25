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

#include <stdlib.h>
#include "channel.h"
#include "slidingwinavg.h"

USING_NAMESPACE


SlidingWindowAverageNode::SlidingWindowAverageNode(int windowSize)
{
    array = NULL;
    if (windowSize<1 || windowSize>100000)
        throw opp_runtime_error("slidingwinavg: invalid window size %d", windowSize);
    winsize = windowSize;
    array = new Datum[winsize];
    count = pos = 0;
    sum = 0;
}

SlidingWindowAverageNode::~SlidingWindowAverageNode()
{
    delete array;
}

bool SlidingWindowAverageNode::isReady() const
{
    return in()->length()>0;
}

void SlidingWindowAverageNode::process()
{
    do {
        Datum& arrayElem = array[pos];
        if (count<winsize)
            count++; // just starting up, filling window [0..pos]
        else
            sum -= arrayElem.y;
        in()->read(&arrayElem,1);
        sum += arrayElem.y;
        Datum o;
        o.x = arrayElem.x;
        o.y = sum/count;
        out()->write(&o,1);
        pos = (pos+1) % winsize;
    }
    while (in()->length()>0);
}

//-----

const char *SlidingWindowAverageNodeType::getDescription() const
{
    return "Replaces every value with the mean of values in the window:\n"
           "yout[k] = SUM(y[i],i=k-winsize+1..k)/winsize";
}

void SlidingWindowAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["windowSize"] = "window size";
}

void SlidingWindowAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["windowSize"] = "10";
}

Node *SlidingWindowAverageNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *ws = attrs["windowSize"].c_str();
    int windowSize = atoi(ws);

    Node *node = new SlidingWindowAverageNode(windowSize);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void SlidingWindowAverageNodeType::mapVectorAttributes(/*inout*/StringMap &attrs) const
{
    attrs["type"] = "double";
}

