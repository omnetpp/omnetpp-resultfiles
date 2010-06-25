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

#ifndef __IOUTPUTVECTORMANAGER_H
#define __IOUTPUTVECTORMANAGER_H

#include <stdlib.h>
#include "IOutputVector.h"
#include "ISimulationTimeProvider.h"
#include "IResultManager.h"

/**
 * Interface for recording output vector data. Output vectors are series of
 * (timestamp,value) pairs. It is recommended to have only one instance of
 * IOutputVectorManager for the whole simulation, and simulation objects
 * (components) should have <code>createVector()</code> or similar methods
 * that delegate to the similar method of the global IOutputVectorManager
 * instance. Actual data can be recorded via methods of the IOutputVector
 * instance returned from <code>createVector()</code>.
 *
 * Methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
class IOutputVectorManager : public IResultManager
{
  public:
    /**
     * Creates an output vector for recording the given vector of the given
     * component.
     *
     * @param componentPath  full path name of the component that records the
     *      vector. The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param vectorName  name of the output vector. This should not usually
     *      contain dots.
     * @param attributes  vector attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    virtual IOutputVector *createVector(const char *componentPath, const char *vectorName,
                                         StringMap& attributes) = 0;

    /**
     * Sets the simulation time provider, to be used for generating a timestamp
     * when a value is recorded without an explicit timestamp.
     */
    virtual void setSimtimeProvider(ISimulationTimeProvider *simtimeProvider) = 0;

    /**
     * Returns the simulation time provider, to be used for generating a timestamp
     * when a value is recorded without an explicit timestamp.
     */
    virtual ISimulationTimeProvider *getSimtimeProvider() = 0;

};

#endif
