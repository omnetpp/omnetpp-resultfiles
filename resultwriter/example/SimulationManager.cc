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


#include "SimulationManager.h"
#include "ISimulationTimeProvider.h"
#include "FileOutputScalarManager.h"
#include "FileOutputVectorManager.h"
#include "AlohaServer.h"

using namespace std;

class SimulationTimeProvider : public ISimulationTimeProvider
{
  private:
    double& now;
  public:
    SimulationTimeProvider(double& now) now(now) {}
    long getEventNumber() const { return 0; }
    double getSimulationTime() const { return now; }
};

SimulationManager::SimulationManager(string runID, stringMap & runAttributes,
                                     string resultFilenameBase)
{
    now = 0.0;
    string scalarFileName = resultFilenameBase;
    string vectorFileName = resultFilenameBase;
    scalarFileName += ".sca";
    vectorFileName += ".vec";
    scalarManager = new FileOutputScalarManager(scalarFileName);
    vectorManager = new FileOutputVectorManager(vectorFileName);

    SimulationTimeProvider *p = new SimulationTimeProvider(now);
    vectorManager->setSimtimeProvider(p);

    scalarManager->open(runID, runAttributes);
    vectorManager->open(runID, runAttributes);
}

SimulationManager::~SimulationManager()
{
    delete scalarManager;
    delete vectorManager;

}

void SimulationManager::simulate(double timeLimit)
{
    while ((!fes.empty()) && fes.back()->time < timeLimit)
    {
        Event *event1 = fes.front();
        if (event1)
        {
            this->now = event1->time;
            event1->execute();
        }
        Event *ev = fes.front();
        fes.erase(fes.begin());
    }

    vector<Component *>::iterator iter;
    for (iter = components.begin(); iter != components.end(); ++iter)
    {
        (*iter)->recordSummaryResults();
    }

    scalarManager->close();
    vectorManager->close();
}

void SimulationManager::add(Event *e)
{
    if (fes.empty())
    {
        fes.push_back(e);
    }
    else
    {
        vector<Event *>::iterator iter;
        for (iter = fes.begin(); iter != fes.end(); ++iter)
        {
            if ((*iter)->time > e->time)
            {
                fes.insert(iter, e);
                return;
            }
        }
        fes.push_back(e);
    }
}

