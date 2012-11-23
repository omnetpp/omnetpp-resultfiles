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

summary.omnetpp_dataset <- function(object, ...) {
    d <- object
    structure(list(runs=levels(d$runattrs$runid),
                   configs=unique(as.character(d$runattrs[d$runattrs$attrname=='configname', 'attrvalue'])),
                   modules=sort(unique(c(as.character(d$scalars$module),as.character(d$vectors$module),as.character(d$statistics$module)))),
                   nscalars=ifelse(is.null(d$scalars), 0, nrow(d$scalars)),
                   scalars=sort(unique(as.character(d$scalars$name))),
                   nvectors=ifelse(is.null(d$vectors), 0, nrow(d$vectors)),
                   vectors=sort(unique(as.character(d$vectors$name))),
                   nstatistics=ifelse(is.null(d$statistics), 0, nrow(d$statistics)),
                   statistics=sort(unique(as.character(d$statistics$name)))
                  ),
                  class='omnetpp_dataset_summary')
}

print.omnetpp_dataset_summary <- function(x, ...) {
    catVertical <- function(v, indent) {
      for (i in 1:length(v)) {
        for (j in 1:indent) cat(" ")
        cat(v[i],"\n")
      }
    }
    
    with(x,
      {
         cat("OMNeT++ dataset containing", nscalars, "scalars,", nvectors, "vectors, and", nstatistics, "statistics")
         cat(" of", length(modules), "modules from", length(runs), "runs of", length(configs), "configurations.\n")
         if (length(configs)) {
           cat("  configurations:\n")
           catVertical(configs, 4)
         }
         if (length(runs)) {
           cat("  runs:\n")
           catVertical(runs, 4)
         }
         if (length(modules)) {
           cat("  modules:\n")
           catVertical(modules, 4)
         }
         if (length(scalars)) {
           cat("  scalars:\n")
           catVertical(scalars, 4)
         }
         if (length(vectors)) {
           cat("  vectors:\n")
           catVertical(vectors, 4)
         }
         if (length(statistics)) {
           cat("  statistics:\n")
           catVertical(statistics, 4)
         }
      })
}
