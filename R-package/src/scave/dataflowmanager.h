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

#ifndef _DATAFLOWMANAGER_H_
#define _DATAFLOWMANAGER_H_

#include <vector>
#include "node.h"
#include "progressmonitor.h"
#include "channel.h"

NAMESPACE_BEGIN


/**
 * Controls execution of the data flow network.
 *
 * @see Node, Channel
 */
class SCAVE_API DataflowManager
{
    protected:
        std::vector<Node *> nodes;
        std::vector<Channel *> channels;
        int threshold; // channel buffer upper limit
        int lastnode; // for round robin

    protected:
        // utility called from connect()
        void addChannel(Channel *channel);

        // scheduler function called by execute()
        Node *selectNode();

        // returns true of a node has finished; if so, also closes
        // its input an output channels.
        bool updateNodeFinished(Node *node);

        // returns true if the node is a reader node
        // (i.e. the category of its type is "reader-node")
        bool isReaderNode(Node *node);

        // helper to estimate the total amount of work
        int64 getTotalBytesToBeRead();

    public:
        /**
         * Constructor
         */
        DataflowManager();

        /**
         * Destructor
         */
        ~DataflowManager();

        /**
         * Add a node to the data-flow network.
         */
        void addNode(Node *node);

        /**
         * Connects two Node ports via a Channel object.
         */
        void connect(Port *src, Port *dest);

        /**
         * Executes the data-flow network. That will basically keep
         * calling the process() method of nodes that say they are
         * ready (isReady() method) until they are all done (isFinished()
         * method). If none of them are ready but there are ones which
         * haven't finished yet, the method declares a deadlock.
         */
        void execute(IProgressMonitor *monitor = NULL);

        /**
         * For debugging: dumps the data-flow network on the standard output.
         */
        void dump();
};

NAMESPACE_END


#endif


