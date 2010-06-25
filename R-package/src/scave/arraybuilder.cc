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

#include <memory.h>
#include <string.h>
#include "channel.h"
#include "arraybuilder.h"

USING_NAMESPACE


ArrayBuilderNode::ArrayBuilderNode()
{
    veccapacity = 0;
    veclen = 0;
    xvec = NULL;
    yvec = NULL;
    xpvec = NULL;
    evec = NULL;
    collectEvec = false;
}

ArrayBuilderNode::~ArrayBuilderNode()
{
    delete [] xvec;
    delete [] yvec;
    delete [] xpvec;
    delete [] evec;
}

void ArrayBuilderNode::resize()
{
    size_t newsize = 3*veccapacity/2;

    if (!newsize)
        newsize = 1024;

    double *newxvec = new double[newsize];
    memcpy(newxvec, xvec, veclen*sizeof(double));
    delete [] xvec;
    xvec = newxvec;

    double *newyvec = new double[newsize];
    memcpy(newyvec, yvec, veclen*sizeof(double));
    delete [] yvec;
    yvec = newyvec;

    if (xpvec) {
        BigDecimal *newxpvec = new BigDecimal[newsize];
        memcpy(newxpvec, xpvec, veccapacity*sizeof(BigDecimal));
        delete[] xpvec;
        xpvec = newxpvec;
    }

    if (collectEvec) {
        eventnumber_t *newevec = new eventnumber_t[newsize];
        memcpy(newevec, evec, veccapacity*sizeof(eventnumber_t));
        delete [] evec;
        evec = newevec;
    }

    veccapacity = newsize;
}

bool ArrayBuilderNode::isReady() const
{
    return in()->length()>0;
}

void ArrayBuilderNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        if (veclen==veccapacity)
            resize();
        Assert(xvec && yvec);
        xvec[veclen] = a.x;
        yvec[veclen] = a.y;
        if (!a.xp.isNil()) {
            if (!xpvec)
                xpvec = new BigDecimal[veccapacity];

            xpvec[veclen] = a.xp;
        }
        if (collectEvec)
            evec[veclen] = a.eventNumber;

        veclen++;
    }
}

bool ArrayBuilderNode::isFinished() const
{
    return in()->eof();
}


void ArrayBuilderNode::sort()
{
    // FIXME sort both arrays by "x"
    //std::sort();
}

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

XYArray *ArrayBuilderNode::getArray()
{
    // transfer ownership to XYArray
    XYArray *array = new XYArray(veclen, xvec, yvec, xpvec, evec);
    xvec = yvec = NULL;
    xpvec = NULL;
    evec = NULL;
    veclen = 0;
    return array;
}

//------

const char *ArrayBuilderNodeType::getDescription() const
{
    return "Collects data into an array. The arrays can be accessed as two double* arrays in the C++ API.";
}

void ArrayBuilderNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["collecteventnumbers"] = "false";
}

void ArrayBuilderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["collecteventnumbers"] = "If true event numbers will be present in the output vector";
}

Node *ArrayBuilderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    ArrayBuilderNode *node = new ArrayBuilderNode();
    if (attrs.find("collecteventnumbers") != attrs.end() && !strcmp("true", attrs["collecteventnumbers"].c_str()))
        node->collectEvec = true;
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


