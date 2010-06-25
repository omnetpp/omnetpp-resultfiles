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

plotHistogramChart <- function (data, properties) {

    getProperty <- function(propertyName, defaultValue=NULL) {
      if (is.null(properties[[propertyName]])) defaultValue else properties[[propertyName]]
    }

    # calculate plot area
    histData <- getProperty('Hist.Data', 'Count')
    baseline <- getProperty('Bars.Baseline', NULL)
    showOverflowCell <- getProperty('Hist.ShowOverflowCell', FALSE)

    xs <- sapply(data, function (x) x$breaks)
    xlim <- range(xs, finite=TRUE)
    xlim[[1]] <- as.numeric(getProperty('X.Axis.Min', xlim[[1]]))
    xlim[[2]] <- as.numeric(getProperty('X.Axis.Max', xlim[[2]]))
    ys <- sapply(data, function (x) switch(histData, Count=x$count, Pdf=x$density, Cdf=x$cdf))
    ylim <- range(ys, finite=TRUE)
    ylim[[1]] <- as.numeric(getProperty('Y.Axis.Min', ylim[[1]]))
    ylim[[2]] <- as.numeric(getProperty('Y.Axis.Max', ylim[[2]]))

    # empty plot
    ylog   <- getProperty('Y.Axis.Log', 'false') == 'true'
    plot.new()
    plot.window(xlim, ylim, log=(if (ylog) 'y' else ''))
    Axis(side=1)
    Axis(side=2)

    grid   <- getProperty('Axes.Grid', 'None')
    switch(grid,
      None=NULL,
      Minor=grid(),
      Major=grid(),
      stop("Unsupported Axes.Grid: ", grid)
    )

    # plot histograms
    histBar  <- getProperty('Hist.Bar', 'Solid')
    defaultColors <- rainbow(length(data))
    colors <- mapply(function(name, index) getProperty(paste('Hist.Color', name, sep='/'), defaultColors[[index]]),
                     names(data), seq_along(data))
    for (i in seq_along(data)) {
          hist <- data[[i]]

          y <- switch(histData,
                 Count = hist$count,
                 Pdf   = hist$density,
                 Cdf   = hist$cdf
               )

          switch(histBar,
            Solid   = {
                        nB <- length(hist$breaks)
                        l <- hist$breaks[-nB]
                        r <- hist$breaks[-1L]
                        t <- y
                        b <- 0
                        rect(l, b, r, t, col=colors[i])
                      },
            Outline = {
                        x <- rep(hist$breaks, each=2)
                        y <- c(0, rep(y, each=2), 0)
                        lines(x,y, col=colors[i], lwd=4)
                      }
          )
    }

    # title
    title(main=getProperty('Graph.Title', ''),
          xlab=getProperty('X.Axis.Title'),
          ylab=getProperty('Y.Axis.Title'))

    # legend
    if (getProperty('Legend.Display', 'false') == 'true') {
      legendPosition  <- getProperty('Legend.Position', properties, 'Inside')
      legendAnchoring <- getProperty('Legend.Anchoring', properties, 'North')
      if (legendPosition != 'Inside')
          warning("Legend.Position not supported: ", legendPosition)
      pos <- legendAnchoringToPosition(legendAnchoring)
      legend(pos, names(data), pch=15, col=colors, lty='blank')
    }

    invisible()
}

