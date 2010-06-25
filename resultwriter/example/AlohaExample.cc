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

#pragma warning(disable:4996)

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include "OutputFileManager.h"
#include "IResultAttributes.h"
#include "SimulationManager.h"
#include "AlohaServer.h"
#include "AlohaHost.h"
#include "FileOutputScalarManager.h"

using namespace std;

class AlohaExample
{
  public:
    stringMap runAttributes1;
    long GOOD_SEEDS[5];   //FIXME why not static

    AlohaExample(const stringMap & str);
    AlohaExample();
    ~AlohaExample();
    void run();
    void makeRunAttributes(stringMap & attributes, int runNumber, int numHosts,
                           double interarrivalTime, int trial);
  private:
    void simulateAloha(int runNumber, int numHosts, double interarrivalTime, int trial);
};

AlohaExample::AlohaExample()
{
    runAttributes1 = stringMap();

    GOOD_SEEDS[0] = 634;
    GOOD_SEEDS[1] = 3424;
    GOOD_SEEDS[2] = 744;
    GOOD_SEEDS[3] = 2525;
    GOOD_SEEDS[4] = 852;
}

AlohaExample::AlohaExample(const stringMap & runAttributes)
{
    this->runAttributes1 = runAttributes;
    GOOD_SEEDS[0] = 634;
    GOOD_SEEDS[1] = 3424;
    GOOD_SEEDS[2] = 744;
    GOOD_SEEDS[3] = 2525;
    GOOD_SEEDS[4] = 852;
}

AlohaExample::~AlohaExample()
{
}

void AlohaExample::run()
{
    //simulateAloha(0,10,5.0,0);
    int runNumber = 0;
    int arrayInt[3] = { 10, 15, 20 };
    double arrayDouble[7] = { 1, 2, 3, 4, 5, 7, 9 };

    for (int numHosts = 0; numHosts < 3; numHosts++)
        for (int interarrivalTime = 0; interarrivalTime < 7; interarrivalTime++)
            for (int trial = 0; trial < 2; trial++)
                simulateAloha(runNumber++, arrayInt[numHosts], arrayDouble[interarrivalTime], trial);
}

void AlohaExample::simulateAloha(int runNumber, int numHosts, double interarrivalTime, int trial)
{
    char ch[500];
    sprintf(ch, "aloha-%d", runNumber);
    cout << "Run #" << runNumber << ": numHosts=" << numHosts << ", interrarivalTime=" <<
        interarrivalTime << "; trial #" << trial << endl;

    FileOutputScalarManager scalarManager;
    string runID = scalarManager.generateRunID(ch);
    stringMap runAttributes;
    makeRunAttributes(runAttributes, runNumber, numHosts, interarrivalTime, trial);

    sprintf(ch, "aloha-%d", runNumber);
    SimulationManager *sim = new SimulationManager(runID, runAttributes, ch);

    double packetDuration = 0.1;

    Component *top = new Component("Aloha", sim, (Component *) NULL);
    AlohaServer *alohaServer = new AlohaServer("server", sim, top);
    srand(GOOD_SEEDS[trial]);
    double random = rand();
    vector<AlohaHost *> hosts;
    for (int i = 0; i < numHosts; i++)
    {
        sprintf(ch, "host%d", i);
        hosts.push_back(new AlohaHost(ch, sim, top, interarrivalTime, packetDuration, alohaServer, random));
    }

    top->recordScalar("interarrivalTime", interarrivalTime, "s");
    top->recordScalar("numHosts", numHosts);

    sim->simulate(5000.0);

    /*FIXME TODO kell?
    for( vector<Component*>::const_iterator it = sim->components.begin(); it != sim->components.end(); ++it){
       delete *it;
       } */

    delete alohaServer;
    delete top;
    delete sim;

    vector<AlohaHost *>::iterator iter;
    for (iter = hosts.begin(); iter != hosts.end(); ++iter)
        delete *iter;
}

void AlohaExample::makeRunAttributes(stringMap & runAttributes, int runNumber, int numHosts,
                                   double interarrivalTime, int trial)
{
    stringMap attr;
    char ch[100];
    char iterVars[100];
    sprintf(iterVars, "numHosts=%d, interarrivalTime=%f", numHosts, interarrivalTime);

    runAttributes[ATTR_NETWORK] = "Aloha";

    sprintf(ch, "%d", runNumber);
    runAttributes[ATTR_RUNNUMBER] = ch;

    runAttributes[ATTR_EXPERIMENT] = "AlohaChannelUtilization";

    runAttributes[ATTR_MEASUREMENT] = iterVars;

    sprintf(ch, "#%d", trial);
    runAttributes[ATTR_REPLICATION] = ch;

    runAttributes[ATTR_DATETIME] = "SYSDATE"; //FIXME TODO new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date());

    runAttributes[ATTR_PROCESSID] = "PROCESSID"; //FIXME TODO ManagementFactory.getRuntimeMXBean().getName();

    sprintf(ch, "%d", trial);
    runAttributes[ATTR_REPETITION] = ch;

    sprintf(ch, "%d", trial);
    runAttributes[ATTR_SEEDSET] = ch;

    runAttributes[ATTR_ITERATIONVARS] = iterVars;

    sprintf(ch, "%s; trial #%d", iterVars, trial);
    runAttributes[ATTR_ITERATIONVARS2] = ch;

    sprintf(ch, "%d", numHosts);
    runAttributes["numHosts"] = ch;

    sprintf(ch, "%d", interarrivalTime);
    runAttributes["interarrivalTime"] = ch;
}

int main()
{
    AlohaExample example = AlohaExample();
    example.run();
    return 0;
}

