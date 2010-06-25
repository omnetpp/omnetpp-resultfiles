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
#include "channel.h"
#include "teenode.h"

USING_NAMESPACE

Port *TeeNode::addPort()
{
    outs.push_back(Port(this));
    return &(outs.back());
}

bool TeeNode::isReady() const
{
    return in()->length()>0;
}

void TeeNode::process()
{
    Datum d;

    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        in()->read(&d,1);
        for (PortVector::iterator it=outs.begin(); it!=outs.end(); it++)
            (*it)()->write(&d,1);
    }

}

bool TeeNode::isFinished() const
{
    return in()->eof();
}

//-------

const char *TeeNodeType::getDescription() const
{
    return "Splits the input into several output by copying its data.\n";
}

void TeeNodeType::getAttributes(StringMap& attrs) const
{
}

Node *TeeNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new TeeNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *TeeNodeType::getPort(Node *node, const char *portname) const
{
    TeeNode *node1 = dynamic_cast<TeeNode *>(node);
    if (!strcmp(portname,"next-out"))
        return node1->addPort();
    else if (!strcmp(portname,"in"))
        return &(node1->in);
    throw opp_runtime_error("no such port `%s'", portname);
}



