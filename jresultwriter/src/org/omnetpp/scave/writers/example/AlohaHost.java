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

package org.omnetpp.scave.writers.example;

import java.util.Random;

import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IResultManager;


public class AlohaHost extends Component {
    Event startTxEvent;
    Event endTxEvent;
    double interarrivalTime;
    double packetDuration;
    AlohaServer alohaServer;
    Random rng;

    int numPkSent = 0;
    IOutputVector radioStateVector;

    private double exponential(double mean) {
        return -mean * Math.log(rng.nextDouble());
    }

    public AlohaHost(String name, SimulationManager sim, Component parent,
            double interarrivalTime, double packetDuration,
            AlohaServer alohaServer, Random rng) {
        super(name, sim, parent);
        this.interarrivalTime = interarrivalTime;
        this.packetDuration = packetDuration;
        this.alohaServer = alohaServer;
        this.rng = rng;

        radioStateVector = createOutputVector("radioState", null, null, IResultManager.IM_SAMPLE_HOLD);
        radioStateVector.record(0); //"idle"

        startTxEvent = new Event() {
            void execute() { startTransmission(); }
        };
        endTxEvent = new Event() {
            void execute() { endTransmission(); }
        };

        schedule(exponential(interarrivalTime), startTxEvent);
    }

    protected void startTransmission() {
        //System.out.println("t=" + now() +" " + getFullPath() + ": begin tx");
        alohaServer.packetReceptionStart();
        schedule(now() + packetDuration, endTxEvent);
        radioStateVector.record(1); //"tx"
    }

    protected void endTransmission() {
        //System.out.println("t=" + now() +" " + getFullPath() + ": end tx");
        alohaServer.packetReceptionEnd();
        numPkSent++;
        schedule(now() + exponential(interarrivalTime), startTxEvent);
        radioStateVector.record(0); //"idle"
    }

    @Override
    protected void recordSummaryResults() {
        recordScalar("pkSent", numPkSent);
    }
}
