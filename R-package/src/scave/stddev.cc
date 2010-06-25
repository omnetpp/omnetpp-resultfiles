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

#include <math.h>
#include "scaveutils.h"
#include "channel.h"
#include "stddev.h"

USING_NAMESPACE


StddevNode::StddevNode()
    : num_vals(0), min_vals(NaN), max_vals(NaN), sum_vals(0.0), sqrsum_vals(0.0)
{
}

StddevNode::~StddevNode()
{
}

bool StddevNode::isReady() const
{
    return in()->length()>0;
}

void StddevNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        collect(a.y);
    }
}

bool StddevNode::isFinished() const
{
    return in()->eof();
}

void StddevNode::collect(double val)
{
    if (++num_vals <= 0)
        throw opp_runtime_error("StddevNode: observation count overflow");

    sum_vals += val;
    sqrsum_vals += val*val;

    if (num_vals>1)
    {
        if (val<min_vals)
            min_vals = val;
        else if (val>max_vals)
            max_vals = val;
    }
    else
    {
        min_vals = max_vals = val;
    }
}

double StddevNode::getVariance() const
{
    if (num_vals<=1)
        return 0.0;
    else
    {
        double devsqr = (sqrsum_vals - sum_vals*sum_vals/num_vals)/(num_vals-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double StddevNode::getStddev() const
{
    return sqrt( getVariance() );
}


//------

const char *StddevNodeType::getDescription() const
{
    return "Collects basic statistics like min, max, mean, stddev.";
}

void StddevNodeType::getAttributes(StringMap& attrs) const
{
}

Node *StddevNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new StddevNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


