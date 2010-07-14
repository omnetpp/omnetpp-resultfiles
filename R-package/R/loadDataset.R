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

add <- function(type=NULL, select=NULL) {
  list(quote(add), type=type, select=select)
}

discard <- function(type=NULL, select=NULL) {
  list(quote(discard), type=type, select=select)
}

loadDataset <- function(files, ...) {
  files <- unlist(sapply(files, Sys.glob), use.names=FALSE)
  commands <- list(...)
  dataset <- .Call('callLoadDataset', files, commands)

  if (is.null(dataset))
    return(dataset)

  structure(
    list(
      runattrs = as.data.frame(dataset$runattrs),
      fileruns = as.data.frame(dataset$fileruns),
      scalars = as.data.frame(dataset$scalars),
      vectors = as.data.frame(dataset$vectors),
      statistics = as.data.frame(dataset$statistics),
      fields = as.data.frame(dataset$fields),
      bins = as.data.frame(dataset$bins),
      params = as.data.frame(dataset$params),
      attrs = as.data.frame(dataset$attrs)
    ),
    class='omnetpp_dataset'
  )
}

