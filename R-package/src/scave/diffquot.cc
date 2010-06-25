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

#include "channel.h"
#include "diffquot.h"

USING_NAMESPACE


DifferenceQuotientNode::DifferenceQuotientNode()
{
    firstRead = false;
}

bool DifferenceQuotientNode::isReady() const
{
    return in()->length() > 0;
}

void DifferenceQuotientNode::process()
{
    Datum r, o;

    if (!firstRead)
    {
        in()->read(&l, 1);
        firstRead = true;
    }

    do {
        in()->read(&r, 1);
        // Make sure that the second values has a later time value
        if (r.x > l.x)
        {
            o.x = l.x;
            o.y = (r.y - l.y) / (r.x - l.x);
            out()->write(&o,1);
            l = r;
        }
    }
    while (in()->length() > 0);
}

//-----

const char *DifferenceQuotientNodeType::getDescription() const
{
    return "Calculates the difference quotient of every value and the subsequent one:\n"
           "yout[k] = (y[k+1]-y[k]) / (t[k+1]-t[k])";
}

void DifferenceQuotientNodeType::getAttributes(StringMap& attrs) const
{
}

void DifferenceQuotientNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *DifferenceQuotientNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new DifferenceQuotientNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void DifferenceQuotientNodeType::mapVectorAttributes(/*inout*/StringMap &attrs) const
{
    attrs["type"] = "double";
}

