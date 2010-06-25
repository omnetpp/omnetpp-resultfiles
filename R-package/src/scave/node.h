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

#ifndef _NODE_H_
#define _NODE_H_

#include "scavedefs.h"
#include "commonutil.h"

NAMESPACE_BEGIN


class Node;
class NodeType;
class Channel;
class DataflowManager;

/**
 * Represents a data element in an output vector file. Processing nodes
 * (Node) process elements of this type.
 *
 * @see Node, Channel, Port
 */
struct Datum
{
    // x value stored as a double or a BigDecimal, usually the simulation time t
    double x;       // x value as double
    BigDecimal xp;  // precise x value, BigDecimal::Nil if not filled in
    double y;       // usually the value at t
    eventnumber_t eventNumber;

    Datum() : xp(BigDecimal::Nil), eventNumber(-1) {}
};


/**
 * Connection point of channels in processing nodes.
 *
 * @see Datum, Node, Channel
 */
class SCAVE_API Port
{
    private:
        Node *ownernode;
        Channel *chan;
    public:
        Port(Node *owner) {ownernode = owner; chan = 0;}
        Port(const Port& p) {ownernode = p.ownernode; chan = p.chan;}
        ~Port() {}
        void setChannel(Channel *channel) {Assert(!chan); chan = channel;}
        Node *getNode() {return ownernode;}
        Channel *getChannel() const  {return chan;}
        Channel *operator()() const {Assert(chan); return chan;}
};


/**
 * Processing node. Processing nodes can be connected via ports and channels
 * to form a data flow network.
 *
 * @see DataflowManager, Port, Channel, Datum, NodeType
 */
class SCAVE_API Node
{
        friend class DataflowManager;

    private:
        DataflowManager *mgr;
        NodeType *nodetype;
        bool alreadyfinished;

    protected:
        /**
         * Called when the node is added to the dataflow manager.
         */
        void setDataflowManager(DataflowManager *m) {mgr = m;}

    public:
        /**
         * Constructor
         */
        Node() {mgr=NULL; nodetype=NULL; alreadyfinished=false;}

        /**
         * Virtual destructor
         */
        virtual ~Node() {}

        /**
         * Returns the dataflow manager in which this node is inserted.
         */
        DataflowManager *dataflowManager() const {return mgr;}

        /**
         * Should be called right after construction by the corresponding
         * NodeType class.
         */
        void setNodeType(const NodeType *t)  {nodetype = const_cast<NodeType *>(t);}

        /**
         * Returns the corresponding NodeType class.
         */
        NodeType *getNodeType() const {return nodetype;}

        /** Execution and scheduling */
        //@{
        /**
         * Do some amount of work, then return
         */
        virtual void process() = 0;

        /**
         * Are more invocations of process() necessary()?
         */
        virtual bool isFinished() const = 0;

        /**
         * Provided it has not isFinished() yet, can process() be invoked
         * right now? (isFinished() is called first -- isReady() is only
         * invoked if isFinished() returns false. So isReady() doesn't need
         * to check for eof.)
         */
        virtual bool isReady() const = 0;
        //@}

        /**
         * Invoked by the dataflow manager when the node's isFinished() first
         * returns true. It sets a status flag so that further invocations
         * of isFinished() can be avoided.
         */
        void setAlreadyFinished() {alreadyfinished = true;}

        /**
         * Used by the dataflow manager. Returns true if setAlreadyFinished()
         * has already been invoked.
         */
        bool getAlreadyFinished() {return alreadyfinished;}
};

NAMESPACE_END


#endif


