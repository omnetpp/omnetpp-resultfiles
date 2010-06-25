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

#include <stddef.h>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include "OutputFileManager.h"
#include "FileOutputScalarManager.h"
#include "FileOutputVectorManager.h"

/*
using namespace std;

int main(){

    FileOutputScalarManager scalar;
    string runID = scalar.generateRunID("test1");
    IOutputScalarManager *scalarManager = new FileOutputScalarManager("1.sca");
    stringMap map;
    scalarManager->open(runID, map);
    scalarManager->recordScalar("top.node1", "pk-sent", 632, map);
    scalarManager->recordScalar("top.node2", "pk-rcvd", 578, map);
    scalarManager->close();

    IOutputVectorManager *vectorManager = new FileOutputVectorManager("1");
    vectorManager->open(runID, map);

    vector<IOutputVector*> vectors;
    for (int i=0; i<10000; i++) {
        double random = rand()/RAND_MAX;
        if (random < 0.001){
            char ch1[20];
            char ch2[20];

            sprintf(ch1, "top.node %d", i%10);
            sprintf(ch2, "eed-% d", (int)(random*10));
            vectors.push_back(vectorManager->createVector(ch1, ch2, map));
        }
        if (!vectors.empty()) {
            int pos = (int)rand()/RAND_MAX*vectors.size();
            IOutputVector *v = vectors[pos];
            v->record(i, 0.1001*i);
        }
    }
    for(int i = 0; i < vectors.size(); i++){
        delete vectors[i];
    }
    vectors.clear();

    delete scalarManager;
    delete vectorManager;
}*/
