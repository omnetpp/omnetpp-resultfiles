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

# input:  scalars, xdata, iso params, average-replications
# output: list of two column matrices (x,y), elements named by series key
createScatterChartDataset <- function (dataset, xModule, xName, isoModules=character(0), isoNames=character(0), isoAttrs=character(0), averageReplications=FALSE) {

  # group scalars according to iso scalars/attributes
  groupScalars <- function(scalars) {
    # remove iso scalars
    i1 <- match(scalars$module, isoModules, nomatch=0)
    i2 <- match(scalars$name, isoNames, nomatch=-1)
    isoScalarIndeces <- which(i1 == i2)
    isoScalars <- scalars[isoScalarIndeces,]
    otherScalars <- if(length(isoScalarIndeces)>0) scalars[-isoScalarIndeces,] else scalars 
    
    # join run attributes and iso scalars
    # TODO iso scalars
    scalarsWithRuns <- merge(omnetpp:::getRunsInWideFormat(dataset$runattrs), otherScalars, by='runid')
    
    # split according to the values of the iso attributes
    s <- if (length(isoAttrs)>0)
           split(scalarsWithRuns, scalarsWithRuns[[isoAttrs]])
         else
           list(scalarsWithRuns)
    
    if (averageReplications) {
      lapply(s,
             function(data) {
               s <- aggregate(data[,'value'], data[,c('experiment','measurement','module','name')], mean)
               # rename column 'x' to 'value'
               names(s)[names(s)=='x'] <- 'value'
               s
             })
    }
    else {
      lapply(s, subset, select=c('runid', 'module', 'name', 'value'))
    }
  }

  # sort one group of scalars
  sortScalars <- function(scalars) {
      xScalarIndeces <- which(scalars$module == xModule & scalars$name == xName)
      xScalars <- scalars[xScalarIndeces,]
      otherScalars <- scalars[-xScalarIndeces,]
      byColumns <- if(averageReplications) c('experiment', 'measurement') else 'runid'
      data <- merge(xScalars, otherScalars, by=byColumns)
      lines <- split(data,list(data$module.y,data$name.y))
      lines <- lapply(lines,
              function(line) {
                  m <- as.matrix(line[c('value.x','value.y')])
                  colnames(m) <- c('x','y')
                  m[order(m[,1]),]
              })
      lines <- lines[sapply(lines,length) > 0]
      lines
  }
  
  # TODO line names
  #   '<module> <name> - <isoModule> <isoName>=<value> <isoAttr>=<value>'
  do.call(c, lapply(groupScalars(dataset$scalars), sortScalars))
}
