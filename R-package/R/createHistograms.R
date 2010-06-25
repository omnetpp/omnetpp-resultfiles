#
# Copyright (c) 2010 Opensim Ltd.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Opensim Ltd. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

createHistograms <- function (dataset) {
  makeHist <- function(key, name) {
      bins <- dataset$bins[dataset$bins$histogram_key == key,]
      # remove under/overflow bins
      overflowCellIndex <- which(!is.finite(bins$lowerbound))
      overflowCount <- if (is.null(overflowCellIndex)) 0 else bins$count[overflowCellIndex]
      underflowCellIndex <- which(!is.finite(bins$upperbound))
      underflowCount <- if (is.null(underflowCellIndex)) 0 else bins$count[underflowCellIndex]
      bins <- bins[-c(overflowCellIndex, underflowCellIndex),]

      counts <- bins$count
      totalCount <- base::sum(counts)
      breaks <- c(bins$lowerbound, bins$upperbound[length(bins$upperbound)])
      widths <- diff(breaks)
      density <- counts / (totalCount * widths)
      cdf  <- (cumsum(counts) / totalCount)
      mids <- (breaks[-length(breaks)] + breaks[-1]) / 2
      equidist <- { x <- diff(breaks) ; diff(range(x)) < 1e-7 * base::mean(x) }
      xmin <- min(breaks) # 'min' field, if there is underflow
      xmax <- max(breaks) # 'max' field, if there is overflow

      structure(list(# histogram fields
                     breaks = breaks,
                     counts = counts,
                     intensities = density,
                     density = density,
                     mids = mids,
                     xname = name,
                     equidist = equidist,

                     # omnetpp_histogram extensions
                     totalCount = totalCount,
                     underflowCount = underflowCount,
                     overflowCount = overflowCount,
                     cdf = cdf,
                     range = c(xmin, xmax)),
              class=c('omnetpp_histogram', 'histogram'))

  }

  data <- merge(getRunsInWideFormat(dataset$runattrs), dataset$histograms)
  histogramNames <- getResultItemNames(data)
  hs <- lapply(seq_along(histogramNames),
               function (i) {
                 name <- histogramNames[i]
                 key  <- data$histogram_key[i]
                 makeHist(key, name)
               })
  names(hs) <- histogramNames
  hs
}
