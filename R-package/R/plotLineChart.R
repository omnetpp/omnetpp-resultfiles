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

plotLineChart <- function (data, properties) {

    getProperty <- function(propertyName, defaultValue=NULL) {
      if (is.null(properties[[propertyName]])) defaultValue else properties[[propertyName]]
    }

    getLineProperty <- function(propertyName, lineName, defaultValue=NULL) {
      getProperty(paste(propertyName, lineName, sep='/'), defaultValue)
    }

    getLineType <- function(lineName) {
      lineType <- getLineProperty('Line.Type', lineName, 'Linear')
      switch(lineType,
        Linear='o',
        Pins='h',
        Dots='p',
        Points='p',
        SampleHold='s',
        BackwardSampleHold='S',
        stop("Unsupported Line.Type: ", lineType)
      )
    }

    getSymbolType <- function(lineName) {
      symbolType <- getLineProperty('Symbols.Type', lineName, 'Dot')
      switch(symbolType,
        None = 0,
        Cross = 4,
        Diamond = 18,
        Dot = 46,
        Plus = 3,
        Square = 15,
        Triangle = 17,
        stop("Unsupported Symbols.Type", symbolType)
      )
    }

    # calculate plot area
    data <- lapply(data, as.data.frame)
    xy <- do.call(rbind, data)
    xlim <- range(xy$x, finite=TRUE)
    ylim <- range(xy$y, finite=TRUE)

    # filter out invisible lines
    visible <- sapply(names(data), function(lineName) {getLineProperty('Line.Display', lineName, 'true') == 'true'})
    data <- data[visible]
    lineNames   <- names(data)

    # plot empty graph (title and axes)
    ylog   <- getProperty('Y.Axis.Log', 'false') == 'true'
    plot.new()
    plot.window(xlim, ylim, log=(if (ylog) 'y' else ''))
    Axis(side=1)
    Axis(side=2)
    box()

    grid   <- getProperty('Axes.Grid', 'None')
    switch(grid,
      None=NULL,
      Minor=grid(),
      Major=grid(),
      stop("Unsupported Axes.Grid: ", grid)
    )

    # plot lines
    lineTypes   <- sapply(lineNames, getLineType)
    lineSymbols <- sapply(lineNames, getSymbolType)
    defaultColors <- rainbow(length(data))
    lineColors <- mapply(function(name, index) getLineProperty('Line.Color', name, defaultColors[[index]]),
                         lineNames, seq_along(lineNames))
    for (i in seq_along(data)) {
      lines(data[[i]]$x, data[[i]]$y, type=lineTypes[i], col=lineColors[i], pch=lineSymbols[i])
    }

    # title
    title(main=getProperty('Graph.Title', ''),
          xlab=getProperty('X.Axis.Title'),
          ylab=getProperty('Y.Axis.Title'))

    # legend
    if (getProperty('Legend.Display', 'false') == 'true') {
      legendPosition  <- getProperty('Legend.Position', 'Inside')
      legendAnchoring <- getProperty('Legend.Anchoring', 'North')

      if (legendPosition != 'Inside')
          warning("Legend.Position not supported: ", legendPosition)
      pos <- legendAnchoringToPosition(legendAnchoring)
      legend(pos, lineNames, pch=lineSymbols, col=lineColors, lty='solid')
    }

    invisible()
}

