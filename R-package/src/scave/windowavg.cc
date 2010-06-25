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
#include "scaveutils.h"
#include "windowavg.h"

USING_NAMESPACE


WindowAverageNode::WindowAverageNode(int windowSize)
{
    array = NULL;
    if (windowSize<1 || windowSize>100000)
        throw opp_runtime_error("winavg: invalid window size %d", windowSize);
    winsize = windowSize;
    array = new Datum[winsize];
}

WindowAverageNode::~WindowAverageNode()
{
    delete array;
}

bool WindowAverageNode::isReady() const
{
    return in()->length()>=winsize || in()->isClosing();
}

void WindowAverageNode::process()
{
    // do as many winsize-sized chunks as possible, but in the end
    // we'll have to do the remaining ones (n<winsize)
    do {
        int n = in()->read(array,winsize);
        double sumy = 0;
        for (int i=0; i<n; i++)
            sumy += array[i].y;
        Datum o;
        o.x = array[0].x;
        o.y = sumy/n;
        out()->write(&o,1);
    }
    while (in()->length()>=winsize || (in()->isClosing() && in()->length()>0));
}

//-----

const char *WindowAverageNodeType::getDescription() const
{
    return "Calculates batched average: replaces every `winsize' input values\n"
           "with their mean. Time is the time of the first value in the batch.";
}

void WindowAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["windowSize"] = "size of batch as integer";
}

void WindowAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["windowSize"] = "10";
}

Node *WindowAverageNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *ws = attrs["windowSize"].c_str();
    int windowSize = atoi(ws);

    Node *node = new WindowAverageNode(windowSize);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void WindowAverageNodeType::mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}


//------------------------------

TimeWindowAverageNode::TimeWindowAverageNode(simultime_t windowSize)
{
    winsize = windowSize;
    win_end = windowSize;
    sum = 0.0;
    count = 0;
}

TimeWindowAverageNode::~TimeWindowAverageNode()
{
}

bool TimeWindowAverageNode::isFinished() const
{
    return in()->eof() && count == 0;
}

bool TimeWindowAverageNode::isReady() const
{
    return in()->length() > 0 || (in()->eof() && count > 0);
}

void TimeWindowAverageNode::process()
{
    int n = in()->length();

    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        if (inCurrentWindow(d))
        {
            collect(d);
        }
        else
        {
            outputWindowAverage();
            moveWindow(d);
            collect(d);
        }
    }

    if (in()->eof())
        outputWindowAverage();
}

//-----

const char *TimeWindowAverageNodeType::getDescription() const
{
    return "Calculates time average: replaces input values in every `windowSize' interval with their mean.\n"
           "tout[k] = k * winSize\n"
           "yout[k] = average of y values in the [(k-1)*winSize, k*winSize) interval";
}

void TimeWindowAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["windowSize"] = "size of window in seconds";
}

void TimeWindowAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["windowSize"] = "1";
}

Node *TimeWindowAverageNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *ws = attrs["windowSize"].c_str();
    simultime_t windowSize;
    parseSimtime(ws, windowSize);
    if (windowSize.isNil() || windowSize <= 0)
        windowSize = 1;

    Node *node = new TimeWindowAverageNode(windowSize);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void TimeWindowAverageNodeType::mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}
