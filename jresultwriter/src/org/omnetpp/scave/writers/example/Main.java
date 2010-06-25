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

import java.lang.management.ManagementFactory;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import org.omnetpp.scave.writers.IResultManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;

public class Main {
    static final long[] GOOD_SEEDS = new long[]{634,3424,744,2525,852}; // completely made-up numbers

    public static void main(String[] args) {
//        simulateAloha(0, 10, 5.0, 0);


///*Uncomment the following and try a full experiment:
        int runNumber = 0;
        for (int numHosts : new int[]{10, 15, 20})
            for (double interarrivalTime : new double[]{1, 2, 3, 4, 5, 7, 9})
                for (int trial=0; trial<2; trial++)
                    simulateAloha(runNumber++, numHosts, interarrivalTime, trial);
//*/
    }

    public static void simulateAloha(int runNumber, int numHosts, double interarrivalTime, int trial) {
        System.out.println("Run #" + runNumber+ ": numHosts="+numHosts + ", interarrivalTime=" + interarrivalTime + "; trial #"+trial);

        String runID = FileOutputScalarManager.generateRunID("aloha-"+runNumber);
        Map<String, String> runAttributes = makeRunAttributes(runNumber, numHosts, interarrivalTime, trial);
        SimulationManager sim = new SimulationManager(runID, runAttributes, "./aloha-"+runNumber);

        double packetDuration = 0.1;

        Component top = new Component("Aloha", sim, null);
        AlohaServer alohaServer = new AlohaServer("server", sim, top);
        Random random = new Random(GOOD_SEEDS[trial]);
        for (int i=0; i<numHosts; i++)
            new AlohaHost("host"+i, sim, top, interarrivalTime, packetDuration, alohaServer, random);

        top.recordScalar("interarrivalTime", interarrivalTime, "s");
        top.recordScalar("numHosts", numHosts);

        sim.simulate(5000.0);
    }

    private static Map<String, String> makeRunAttributes(int runNumber, int numHosts, double interarrivalTime, int trial) {
        String iterVars = "numHosts="+numHosts + ", interarrivalTime=" + interarrivalTime;
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "Aloha");
        runAttributes.put(IResultManager.ATTR_RUNNUMBER, String.valueOf(runNumber));
        runAttributes.put(IResultManager.ATTR_EXPERIMENT, "AlohaChannelUtilization");
        runAttributes.put(IResultManager.ATTR_MEASUREMENT, iterVars);
        runAttributes.put(IResultManager.ATTR_REPLICATION, "#"+trial);
        runAttributes.put(IResultManager.ATTR_DATETIME, new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date()));
        runAttributes.put(IResultManager.ATTR_PROCESSID, ManagementFactory.getRuntimeMXBean().getName());
        runAttributes.put(IResultManager.ATTR_REPETITION, String.valueOf(trial));
        runAttributes.put(IResultManager.ATTR_SEEDSET, String.valueOf(trial));
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS, iterVars);
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS2, iterVars+"; trial #"+trial);
        runAttributes.put("numHosts", String.valueOf(numHosts));
        runAttributes.put("interarrivalTime", String.valueOf(interarrivalTime));
        return runAttributes;
    }

}
