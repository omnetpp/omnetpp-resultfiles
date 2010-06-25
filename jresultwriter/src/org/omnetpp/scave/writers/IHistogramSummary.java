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

/**
 * Interface for presenting histogram data.
 *
 * @author Andras
 */
public interface IHistogramSummary extends IStatisticalSummary {
    /**
     * Returns the number of histogram cells used.
     */
    int getNumCells();

    /**
     * Returns the kth cell boundary. Legal values for k are 0 through getNumCells(),
     * i.e. there are <code>getNumCells()+1</code> cell boundaries.
     * <code>getCellBounds(0)</code> returns the lower bound of the first cell,
     * and <code>getCellBounds(getNumCells())</code> returns the upper bound of the
     * last cell.
     */
    double getCellBoundary(int k);

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    double getCellValue(int k);

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
     */
    double getCellPDF(int k);

    /**
     * Returns number of observations that, being too small, fell out of the histogram
     * range.
     */
    long getUnderflowCell();

    /**
     * Returns number of observations that, being too large, fell out of the histogram
     * range.
     */
    long getOverflowCell();
}
