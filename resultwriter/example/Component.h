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

#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <vector>
#include "Event.h"

class SimulationManager;

class Component
{
  protected:
    Component *parent;
    std::string name;
    std::string fullPath;
    friend class SimulationManager;
    SimulationManager *sim;

  public:
    Component(const char *name, SimulationManager *sim, Component *parent);

    virtual ~Component() {}

    virtual const char *getFullPath();

    virtual void schedule(double time, Event *e);

    virtual double now();

    virtual void recordScalar(const char *name, double value);

    virtual void recordScalar(const char *name, double value, const char *unit);

    virtual IOutputVector *createOutputVector(const char *name);

    virtual IOutputVector *createOutputVector(const char *name, const char *unit, const char *type,
                                      const char *interpolationMode);

    virtual IOutputVector *createOutputVector(const char *name, const char *enumName,
                                      std::vector<std::string> names, std::vector<int> values);

    virtual IOutputVector *createOutputVector(const char *name, StringMap& attributes);

    virtual void recordSummaryResults();

  protected:
    // OVERRIDE AND ADD recordScalar() CALLS
    virtual void recordScalar(const char *name, double value, conat StringMap attributes);
};

#endif
