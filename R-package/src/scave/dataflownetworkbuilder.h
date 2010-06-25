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

#ifndef _DATAFLOW_NETWORK_BUILDER_H_
#define _DATAFLOW_NETWORK_BUILDER_H_

#include <iostream>
#include <map>

#include "resultfilemanager.h"
#include "arraybuilder.h"

typedef enum {Apply, Compute} ProcessingOperationType;

struct ProcessingOperation {
    ProcessingOperationType type;
    std::string select;
    Computation *computation;

    ProcessingOperation(ProcessingOperationType type, const char *select, Computation *computation)
        : type(type), select(select), computation(computation) {}
};

typedef std::vector<ProcessingOperation> ProcessingOperationList;
typedef std::vector<ArrayBuilderNode*> ArrayBuilderNodes;

class DataflowNetworkBuilder
{
private:
    ResultFileManager &resultFileManager;
    DataflowManager *dataflowManagerPtr;
    IDList outputIDs;
    ArrayBuilderNodes arrayBuilderNodes; // has the same size as outputIDs!

public:

    DataflowNetworkBuilder(ResultFileManager &manager) : resultFileManager(manager), dataflowManagerPtr(NULL) {}
    ~DataflowNetworkBuilder();

    void build(const IDList &input, const ProcessingOperationList &operations);

    DataflowManager* getDataflowManager() const { return dataflowManagerPtr; }
    IDList getOutputIDs() const { return outputIDs; }
    ArrayBuilderNodes getArrayBuilderNodes() const { return arrayBuilderNodes; }
};

#endif
