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
makeScatterChartDataset <- function (dataset, xScalar, isoScalars=list(), isoAttrs=character(0), averageReplications=FALSE) {

  # group scalars according to iso scalars/attributes
  groupScalars <- function(scalars) {
    isoParams <- c(isoAttrs, paste(isoScalars$module, isoScalars$name))
      
    # join run attributes and iso scalars with non-iso scalars
    isIsoScalar <- match(scalars$module, isoScalars$module, nomatch=0) ==
                   match(scalars$name, isoScalars$name, nomatch=-1)
    isoValues <- merge(cast(dataset$runattrs, runid~attrname, value='attrvalue'),
                       cast(transform(scalars[isIsoScalar,], resultkey=NULL, file=NULL, module=NULL, name=paste(module,name)), runid~name),
                       by='runid', all=TRUE)
    scalars <- merge(scalars[!isIsoScalar,], isoValues, by='runid')
                     
    if (is.null(isoParams) || length(isoParams) == 0 || length(setdiff(isoParams, colnames(scalars)))>0)
      return (list(scalars))
    
    # split according to the iso values
    split(scalars, scalars[[isoParams]], drop=TRUE)
  }
  
  # average replications if needed
  averageScalars <- if (averageReplications)
                      function(scalars) rename(cast(scalars, experiment+measurement+module+name~., mean), c('(all)'='value'))
                    else
                      function(scalars) subset(scalars, select=c('runid', 'module', 'name', 'value'))

  # sort one group of scalars
  sortScalars <- function(scalars) {
      isXScalar <- scalars$module == xScalar$module & scalars$name == xScalar$name
      xScalars <- scalars[isXScalar,]
      yScalars <- scalars[!isXScalar,]
      runId <- if(averageReplications) c('experiment', 'measurement') else 'runid'
      lines <- dlply(yScalars, ~module+name,
                     function(yScalars) {
                       d <- merge(xScalars, yScalars, by=runId)[c('value.x','value.y')]
                       as.matrix(arrange(rename(d, c(value.x='x', value.y='y')), x))
                     })
      lines <- lines[sapply(lines,length) > 0]
      lines
  }
  
  # TODO line names
  #   '<module> <name> - <isoModule> <isoName>=<value> <isoAttr>=<value>'
  do.call(c, lapply(lapply(groupScalars(dataset$scalars), averageScalars), sortScalars))
}
