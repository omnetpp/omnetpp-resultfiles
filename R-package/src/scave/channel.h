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

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "commonutil.h"
#include <deque>
#include "node.h"

NAMESPACE_BEGIN


/**
 * Does buffering between two processing nodes (Node).
 *
 * @see Node, Port, Datum
 */
class SCAVE_API Channel
{
    private:
        // note: a Channel should *never* hold a pointer back to its Ports
        // because ports may be copied after having been assigned to channels
        // (e.g. in VectorFileReader which uses std::vector). Node ptrs are OK.
        std::deque<Datum> buffer;  //XXX deque has very poor performance under VC++ (pagesize==1!), consider using std::vector here instead
        Node *producernode;
        Node *consumernode;
        bool producerfinished;
        bool consumerfinished;
    public:
        Channel();
        ~Channel() {}

        void setProducerNode(Node *node) {producernode = node;}
        Node *getProducerNode() const {return producernode;}

        void setConsumerNode(Node *node) {consumernode = node;}
        Node *getConsumerNode() const {return consumernode;}

        /**
         * Returns ptr to the first buffered data item (next one to be read), or NULL
         */
        const Datum *peek() const;

        /**
         * Writes an array.
         */
        void write(Datum *a, int n);

        /**
         * Reads into an array. Returns number of items actually stored.
         */
        int read(Datum *a, int max);

        /**
         * Returns true if producer has already called close() which means
         * there won't be any more data except those already in the buffer
         */
        bool isClosing()  {return producerfinished;}

        /**
         * Returns true if close() has been called and there's no buffered data
         */
        bool eof()  {return producerfinished && length()==0;}

        /**
         * Called by the producer to declare it will not write any more --
         * if also there's no more buffered data (length()==0), that means EOF.
         */
        void close()  {producerfinished=true;}

        /**
         * Called when consumer has finished. Causes channel to ignore
         * further writes (discard any data written).
         */
        void consumerClose() {buffer.clear();consumerfinished=true;}

        /**
         * Returns true when the consumer has closed the channel, that is,
         * it will not read any more data from the channel.
         */
        bool isConsumerClosed() {return consumerfinished;}

        /**
         * Number of currently buffered items.
         */
        int length() {return buffer.size();}
};

NAMESPACE_END


#endif


