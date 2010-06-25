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

#ifndef __ALOHAHOST_H
#define __ALOHAHOST_H

#include "AlohaServer.h"
#include "Component.h"
#include "Event.h"

class AlohaHost : public Component
{
  public:
    Event *startTxEvent;
    Event *endTxEvent;
    double interarrivalTime;
    double packetDuration;
    AlohaServer *alohaServer;
    double random;

    int numPkSent;
    IOutputVector *radioStateVector;
    friend class StartTxEvent;
    friend class EndTxEvent;

  private:
    double exponential(double mean);

  public:
    AlohaHost(const char *name, SimulationManager *sim, Component *parent,
              double interarrivalTime, double packetDuration,
              AlohaServer *alohaServer, double rng);
    ~AlohaHost();

  protected:
    void startTransmission();
    void endTransmission();
    void recordSummaryResults();
};

class StartTxEvent : public Event
{
  private:
    AlohaHost *alohaHost;
  public:
    StartTxEvent(AlohaHost *host);
    void execute();
};

class EndTxEvent : public Event
{
  private:
    AlohaHost *alohaHost;
  public:
    EndTxEvent(AlohaHost *host);
    void execute();
};

#endif

