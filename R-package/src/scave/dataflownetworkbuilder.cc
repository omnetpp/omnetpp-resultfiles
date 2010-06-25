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

#include <iostream>
#include <map>
#include <cstring>

#include "xyarray.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "vectorfilereader.h"
#include "arraybuilder.h"
#include "dataflownetworkbuilder.h"

struct PNode;
struct PPort;
struct PChannel;

typedef std::vector<PNode*> PNodeVector;
typedef std::vector<PPort*> PPortVector;
typedef std::vector<PChannel*> PChannelVector;

struct PNode
{
    NodeType *nodeType;
    StringMap attrs;
    Node *node;
    PPortVector inputPorts;
    PPortVector outputPorts;

    PNode(const char *nodeTypeName, StringMap &attrs);
    ~PNode();
    PPort* addInputPort(const char *name);
    PPort* addOutputPort(const char *name);
};

struct PPort
{
    std::string name;
    PNode *owner;
    PChannel *channel;
    ID id; // for output ports only

    PPort(PNode *owner, const char *name) : name(name), owner(owner), channel(NULL), id(0) {}
};

struct PChannel
{
    PPort *out;
    PPort *in;

    PChannel(PPort *out, PPort *in) : out(out), in(in) {}
};

struct PNetwork
{
    ResultFileManager &resultFileManager;
    PNodeVector nodes;
    PChannelVector channels;
    PPortVector openOutputPorts;

    PNetwork(const IDList &input, ResultFileManager &manager);
    ~PNetwork();

    IDList getOutputIDs();
    ID addComputedVector(ID id, Computation *computation);
    IDList select(const char* pattern);
    void apply(IDList input, Computation *computation);
    void compute(IDList input, Computation *computation);
    void addArrayBuilders();
    ArrayBuilderNodes getArrayBuilderNodes();


    PNode* newNode(const char *nodeTypeName, StringMap &attrs);
    PNode* newNode(Computation *computation);
    void connect(PPort *out, PPort *in);
    DataflowManager* createDataflowNetwork();
};

PNode::PNode(const char *nodeTypeName, StringMap &attrs)
    : attrs(attrs), node(NULL)
{
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
    nodeType = registry->getNodeType(nodeTypeName);
    if (nodeType == NULL)
        throw opp_runtime_error("Processing node '%s' not found in the registry.", nodeTypeName);
}

PNode::~PNode()
{
    for (PPortVector::iterator it = inputPorts.begin(); it != inputPorts.end(); ++it)
        delete (*it);
    for (PPortVector::iterator it = outputPorts.begin(); it != outputPorts.end(); ++it)
        delete (*it);
}

PPort* PNode::addInputPort(const char *name)
{
    PPort *port = new PPort(this, name);
    inputPorts.push_back(port);
    return port;
}

PPort* PNode::addOutputPort(const char *name)
{
    PPort *port = new PPort(this, name);
    outputPorts.push_back(port);
    return port;
}

PNetwork::PNetwork(const IDList &input, ResultFileManager &manager)
    : resultFileManager(manager)
{
    // add reader nodes and ports

    std::map<ResultFile*, PNode*> fileReaders;

    // create reader nodes
    ResultFileList *fileList = resultFileManager.getUniqueFiles(input);
    StringMap attrs;
    for (int i = 0; i < (int)fileList->size(); i++)
    {
        ResultFile *resultFile = fileList->at(i);
        attrs["filename"] = resultFile->fileSystemFilePath;
        PNode *readerNode = newNode("indexedvectorfilereader", attrs);
        fileReaders[resultFile] = readerNode;
    }
    delete fileList;

    // create a reader port for each input vector
    int vectorCount = input.size();
    for (int i = 0; i < vectorCount; i++)
    {
        // create a port for each vector on its reader node
        ID id = input.get(i);
        const VectorResult& vector = resultFileManager.getVector(id);
        char portName[30];
        sprintf(portName, "%d", vector.vectorId);
        PNode *readerNode = fileReaders[vector.fileRunRef->fileRef];
        PPort *outPort = readerNode->addOutputPort(portName);
        outPort->id = id;
        openOutputPorts.push_back(outPort);
    }
}


PNetwork::~PNetwork()
{
    for (PNodeVector::iterator it = nodes.begin(); it != nodes.end(); ++it)
        delete (*it);
    for (PChannelVector::iterator it = channels.begin(); it != channels.end(); ++it)
        delete (*it);
}

PNode* PNetwork::newNode(const char *nodeTypeName, StringMap &attrs)
{
    PNode* node = new PNode(nodeTypeName, attrs);
    nodes.push_back(node);
    return node;
}

PNode* PNetwork::newNode(Computation *computation)
{
    std::string operation = computation->getOperationName();
    StringMap params = computation->getOperationParams();
    return newNode(operation.c_str(), params);
}

void PNetwork::connect(PPort *out, PPort *in)
{
    PChannel *channel = new PChannel(out, in);
    out->channel = channel;
    in->channel = channel;
    channels.push_back(channel);
}

IDList PNetwork::getOutputIDs()
{
    IDList idlist;
    for (PPortVector::iterator it = openOutputPorts.begin(); it != openOutputPorts.end(); ++it)
    {
        PPort *outputPort = *it;
        Assert(outputPort->id != 0);
        idlist.add(outputPort->id);
    }
    return idlist;
}

IDList PNetwork::select(const char* pattern)
{
    IDList source = getOutputIDs();
    return resultFileManager.filterIDList(source, pattern);
}

ID PNetwork::addComputedVector(ID inputID, Computation *computation)
{
    static int nextVectorId = 0;
    const VectorResult inputVector = resultFileManager.getVector(inputID);
    const char *fileName = "|computed|";
    int vectorId = nextVectorId++;
    StringMap vectorAttrs; // TODO
    std::string vectorName = computation->getOperationName() + "(" + (*inputVector.nameRef) + ")";
    ComputationID computationID = computation->getID();
    return resultFileManager.addComputedVector(vectorId, vectorName.c_str(), fileName, vectorAttrs, computationID, inputID, (ComputationNode)computation);
}

void PNetwork::apply(IDList input, Computation *computation)
{
    for (int i = 0; i < (int)openOutputPorts.size(); ++i)
    {
        PPort *outputPort = openOutputPorts[i];
        Assert(outputPort->id != 0);
        if (input.indexOf(outputPort->id) >= 0)
        {
            PNode *filterNode = newNode(computation);
            PPort *filterIn = filterNode->addInputPort("in");
            PPort *filterOut = filterNode->addOutputPort("out");
            connect(outputPort, filterIn);
            filterOut->id = addComputedVector(outputPort->id, computation);
            openOutputPorts[i] = filterOut;
        }
    }
}

void PNetwork::compute(IDList input, Computation *computation)
{
    int size = openOutputPorts.size();
    for (int i = 0; i < size; ++i)
    {
        PPort *outputPort = openOutputPorts[i];
        Assert(outputPort->id != 0);
        if (input.indexOf(outputPort->id) >= 0)
        {
            StringMap emptyAttrs;
            PNode *teeNode = newNode("tee", emptyAttrs);
            PPort *teeIn = teeNode->addInputPort("in");
            PPort *teeOut1 = teeNode->addOutputPort("next-out");
            PPort *teeOut2 = teeNode->addOutputPort("next-out");
            connect(outputPort, teeIn);
            teeOut1->id = outputPort->id;
            teeOut2->id = outputPort->id;
            openOutputPorts[i] = teeOut1;

            PNode *filterNode = newNode(computation);
            PPort *filterIn = filterNode->addInputPort("in");
            PPort *filterOut = filterNode->addOutputPort("out");
            connect(teeOut2, filterIn);
            filterOut->id = addComputedVector(teeOut2->id, computation);
            openOutputPorts.push_back(filterOut);
        }
    }
}

void PNetwork::addArrayBuilders()
{
    for (PPortVector::iterator it = openOutputPorts.begin(); it != openOutputPorts.end(); ++it)
    {
        PPort *outputPort = *it;

        StringMap attrs;
        attrs["collecteventnumbers"] = "true";
        PNode *arrayBuilder = newNode("arraybuilder", attrs);

        PPort *inputPort = arrayBuilder->addInputPort("in");
        connect(outputPort, inputPort);
    }
    openOutputPorts.clear();
}

ArrayBuilderNodes PNetwork::getArrayBuilderNodes()
{
    ArrayBuilderNodes arrayBuilders;
    for (PNodeVector::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        PNode *node = *it;
        if (strcmp("arraybuilder", node->nodeType->getName()) == 0)
        {
            ArrayBuilderNode *arrayBuilder = dynamic_cast<ArrayBuilderNode*>(node->node);
            Assert(arrayBuilder != NULL);
            arrayBuilders.push_back(arrayBuilder);
        }
    }
    return arrayBuilders;
}

DataflowManager* PNetwork::createDataflowNetwork()
{
    DataflowManager *dataflowManager = new DataflowManager;

    // create nodes
    for (PNodeVector::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        PNode *node = *it;
        node->node = node->nodeType->create(dataflowManager, node->attrs);
    }

    // create ports and channels
    for (PChannelVector::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        PChannel *channel = *it;

        PPort *outPort = channel->out;
        PNode *outNode = outPort->owner;
        Port *out = outNode->nodeType->getPort(outNode->node, outPort->name.c_str());

        PPort *inPort = channel->in;
        PNode *inNode = inPort->owner;
        Port *in = inNode->nodeType->getPort(inNode->node, inPort->name.c_str());

        dataflowManager->connect(out, in);
    }

    return dataflowManager;
}

DataflowNetworkBuilder::~DataflowNetworkBuilder()
{
    // TODO
}

void DataflowNetworkBuilder::build(const IDList &input, const ProcessingOperationList &operations)
{
    // create reader nodes and ports
    PNetwork network(input, resultFileManager);

    // create filter nodes
    for (ProcessingOperationList::const_iterator it = operations.begin(); it != operations.end(); ++it)
    {
        ProcessingOperation operation = *it;
        IDList input = network.select(operation.select.c_str());
        switch (operation.type)
        {
        case Apply: network.apply(input, operation.computation); break;
        case Compute: network.compute(input, operation.computation); break;
        }
    }

    // finally add arraybuilders
    outputIDs.set(network.getOutputIDs());
    network.addArrayBuilders();

    // build "real" network
    dataflowManagerPtr = network.createDataflowNetwork();
    arrayBuilderNodes = network.getArrayBuilderNodes();
}
