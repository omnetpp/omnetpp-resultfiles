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

#include "AlohaServer.h"

using namespace std;

AlohaServer::AlohaServer(string name, SimulationManager *sim, Component *parent) : Component(name, sim, parent)
{
    numCurrentTransmissions = 0;
    collision = false;
    totalPackets = 0;
    totalReceiveTime = 0;
    totalCollisionTime = 0;
    numCurrentTransmissionsVector =
        createOutputVector("numConcurrentTransmissions", "", "", IM_SAMPLE_HOLD);
    numCurrentTransmissionsVector->record(0);
}

AlohaServer::~AlohaServer()
{
}

void AlohaServer::packetReceptionStart()
{
    if (numCurrentTransmissions == 0)
        rxStartTime = now();
    else
        collision = true;
    numCurrentTransmissions++;
    numCurrentTransmissionsVector->record(numCurrentTransmissions);

    totalPackets++;
}

void AlohaServer::packetReceptionEnd()
{
    numCurrentTransmissions--;
    numCurrentTransmissionsVector->record(numCurrentTransmissions);
    if (numCurrentTransmissions == 0)
    {
        if (collision)
            totalCollisionTime += now() - rxStartTime;
        else
            totalReceiveTime += now() - rxStartTime;
        collision = false;
    }
}

void AlohaServer::recordSummaryResults()
{
    recordScalar("totalPackets", totalPackets);
    recordScalar("totalCollisionTime", totalCollisionTime, "s");
    recordScalar("totalReceiveTime", totalReceiveTime, "s");
    recordScalar("channelBusy", 100 * (totalReceiveTime + totalCollisionTime) / now(), "%");
    recordScalar("utilization", 100 * totalReceiveTime / now(), "%");
}
