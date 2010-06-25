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
#include <sstream>
#include "SimulationManager.h"
#include "IResultAttributes.h"

using namespace std;

Component::Component(string name, SimulationManager *sim, Component *parent)
{
    this->name = name.c_str();
    this->sim = sim;
    this->parent = parent;
    sim->components.push_back(this);
}

string Component::getFullPath()
{
    string str;
    if (parent)
    {
        string parentName = string(parent->name);
        parentName += ".";
        str += parentName;
        str += string(name);
    }
    else
    {
        str = string(name);
    }
    return str;
}

void Component::schedule(double time, Event *e)
{
    e->time = time;
    sim->add(e);
}

double Component::now()
{
    return sim->now;
}

void Component::recordSummaryResults()
{
    // OVERRIDE AND ADD recordScalar() CALLS
}

void Component::recordScalar(string name, double value, stringMap attributes)
{
    sim->scalarManager->recordScalar(getFullPath(), name, value, attributes);
}

void Component::recordScalar(string name, double value)
{
    stringMap map;
    recordScalar(name, value, map);
}

void Component::recordScalar(string name, double value, string unit)
{
    stringMap map;
    map[ATTR_UNIT] = unit;
    recordScalar(name, value, map);
}

IOutputVector *Component::createOutputVector(string name)
{
    stringMap map = stringMap();
    return createOutputVector(name, map);
}

IOutputVector *Component::createOutputVector(string name, string unit, string type,
                                             string interpolationMode)
{
    stringMap *attributes = new stringMap();
    if (!unit.empty())
        (*attributes)[ATTR_UNIT] = unit;
    if (!type.empty())
        (*attributes)[ATTR_TYPE] = type;
    if (!interpolationMode.empty())
        (*attributes)[ATTR_INTERPOLATIONMODE] = interpolationMode;
    return sim->vectorManager->createVector(getFullPath(), name, *attributes);
}

IOutputVector *Component::createOutputVector(string name, string enumName, vector<string>  names,
                                             vector<int> values)
{
    ostringstream enumDesc;

    for (int i = 0; i < (int)names.size(); i++)
    {
        const char *ch = i == 0 ? "" : ",";
        enumDesc << ch << names[i] << "=" << values[i];
    }
    stringMap attributes;
    attributes[ATTR_ENUMNAME] = enumName;
    attributes[ATTR_ENUM] = enumDesc.str();
    return sim->vectorManager->createVector(getFullPath(), name, attributes);
}

IOutputVector *Component::createOutputVector(string name, stringMap & attributes)
{
    return sim->vectorManager->createVector(getFullPath(), name, attributes);
}
