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

package org.omnetpp.scave.writers.test;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;
import org.omnetpp.scave.writers.impl.FileOutputVectorManager;

public class BasicTest {
    public static void main(String[] args) {
        String runID = FileOutputScalarManager.generateRunID("test1");
        IOutputScalarManager scalarManager = new FileOutputScalarManager("1.sca");
        scalarManager.open(runID, null);
        scalarManager.recordScalar("top.node1", "pk-sent", 632, null);
        scalarManager.recordScalar("top.node2", "pk-rcvd", 578, null);
        scalarManager.close();

        IOutputVectorManager vectorManager = new FileOutputVectorManager("1.vec");
        vectorManager.open(runID, null);

        List<IOutputVector> vectors = new ArrayList<IOutputVector>();
        for (int i=0; i<100000; i++) {
            if (Math.random() < 0.001)
                vectors.add(vectorManager.createVector("top.node"+(i%10), "eed-"+(int)(Math.random()*10), null));
            if (!vectors.isEmpty()) {
                int pos = (int)Math.floor(Math.random()*vectors.size());
                IOutputVector v = vectors.get(pos);
                v.record(i, 0.1001*i);
            }
        }
    }
}
