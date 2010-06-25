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

package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Contains common methods for IOutputScalarManager and IOutputVectorManager.
 * Methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
public interface IResultManager extends IResultAttributes {
    /**
     * Opens collecting. This should be called at the beginning of a simulation run.
     *
     * The runID will be used to identify the run with during result analysis,
     * so, while making sense to the user, it should also be globally unique with
     * a reasonable probability. A recommended runID format is the following:
     * <i>&lt;networkname&gt;-&lt;runnumber&gt;-&lt;datetime&gt;-&lt;hostname&gt;</i>.
     *
     * Attributes contain metadata about the simulation run; see IResultAttributes
     * for attribute names. Many of run attributes are useful for the result
     * analysis process.
     *
     * @param runID  reasonably globally unique string to identify the run.
     * @param attributes  run attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    void open(String runID, Map<String,String> runAttributes);

    /**
     * Close collecting. This should be called at the end of a simulation run.
     */
    void close();

    /**
     * Force writing out all buffered output.
     */
    void flush();

    /**
     * Returns the name of the output scalar/vector file. Returns null if this
     * object is not producing file output.
     */
    String getFileName();
}
