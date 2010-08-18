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
makeScatterChartDataset <- function (dataset, xModule, xName, isoModules=character(0), isoNames=character(0), isoAttrs=character(0), averageReplications=FALSE) {

  # group scalars according to iso scalars/attributes
  groupScalars <- function(scalars) {
    # separate iso scalars
    i1 <- match(scalars$module, isoModules, nomatch=0)
    i2 <- match(scalars$name, isoNames, nomatch=-1)
    isoScalarIndeces <- which(i1 == i2)
    isoScalars <- scalars[isoScalarIndeces,]
    scalars <- if(length(isoScalarIndeces)>0) scalars[-isoScalarIndeces,] else scalars 
    
    # join run attributes and iso scalars
    isoScalarNames <- paste(isoModules, isoNames)
    isoScalars <- data.frame(runids=isoScalars$runid, name=paste(isoScalars$module, isoScalars$name), value=isoScalars$value)
    runAttrs <- cast(dataset$runattrs, runid~attrname, value='attrvalue')
    isoParams <- if (nrow(isoScalars)>0)
                   merge(runAttrs,
                         reshape(isoScalars, direction='wide', idvar='runid', timevar='name'),
                         by='runid')
                 else
                   runAttrs 
    scalars <- merge(isoParams, scalars, by='runid')
    
    # split according to the values of the iso attributes
    isoColumns <- c(isoAttrs, paste(isoModules, isoNames))
    scalars <- if (length(isoColumns)>0 && !is.null(scalars[[isoColumns]])) {
                   split(scalars, scalars[[isoColumns]])
                 }
               else
                 list(scalars)
    
    if (averageReplications) {
      lapply(scalars,
             function(data) {
               if (nrow(data) > 0) {
                 data <- aggregate(data[,'value'], data[,c('experiment','measurement','module','name')], mean)
                 # rename column 'x' to 'value'
                 names(data)[names(data)=='x'] <- 'value'
               }
               data
             })
    }
    else {
      lapply(scalars, subset, select=c('runid', 'module', 'name', 'value'))
    }
  }

  # sort one group of scalars
  sortScalars <- function(scalars) {
      xScalarIndeces <- which(scalars$module == xModule & scalars$name == xName)
      xScalars <- scalars[xScalarIndeces,]
      otherScalars <- scalars[-xScalarIndeces,]
      byColumns <- if(is.null(scalars$runid)) c('experiment', 'measurement') else 'runid'
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
