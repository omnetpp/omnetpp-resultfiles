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

#
# TODO baseline, logarithimic y
#
plotBarChart <- function (data, properties, conf.int=NULL) {
    getProperty <- function(propertyName, defaultValue=NULL) {
      if (is.null(properties[[propertyName]])) defaultValue else properties[[propertyName]]
    }

    getBarProperty <- function(propertyName, barName, defaultValue=NULL) {
      getProperty(paste(propertyName, barName, sep='/'), defaultValue)
    }

    data <- t(data)
    if (!is.null(conf.int))
      conf.int <- t(conf.int)

    # layout, calculate positions of bars
    cGroups <- ncol(data) # number of groups
    cBars <- nrow(data)   # number of bars in each group
    groupIndex <- col(data) - 1
    barIndex   <- row(data) - 1
    barWidth <- 1.0
    hgapMinorPct <- 0.5
    hgapMajorPct <- 0.25
    horizontalInset <- 1.0
    baseline <- 0

    barPlacement <- getProperty('Bar.Placement', 'Aligned')
    bars <- switch(barPlacement,
      Aligned    = {
                     hgapMinor  <- hgapMinorPct * barWidth
                     groupWidth <- barWidth * cBars + (hgapMinor * (cBars - 1))
                     hgapMajor  <- hgapMajorPct * groupWidth
                     l <- groupIndex*(groupWidth+hgapMajor) + barIndex*(barWidth+hgapMinor)
                     list(
                       l = l,
                       r = l+barWidth,
                       t = as.vector(data),
                       b = 0
                     )
                   },
      Overlapped = {
                     groupWidth <- cBars * barWidth
                     hgapMajor <- hgapMajorPct * groupWidth
                     l <- groupIndex * (groupWidth+hgapMajor) + barIndex * barWidth / 2
                     list(
                       l = l,
                       r = l + barWidth * cBars / 2.0,
                       t = as.vector(data),
                       b = 0
                     )
                   },
      Infront    = {
                     groupWidth <- cBars * barWidth
                     hgapMajor <- hgapMajorPct * groupWidth
                     l <- groupIndex*(groupWidth+hgapMajor)
                     list(
                       l = l,
                       r = l + groupWidth,
                       t = as.vector(data),
                       b = 0
                     )
                   },
      Stacked    = {
                     groupWidth <- cBars * barWidth
                     hgapMajor <- hgapMajorPct * groupWidth
                     l <- groupIndex*(groupWidth+hgapMajor)
                     # TODO negative values?
                     h <- base::apply(data, 2, diffinv)
                     list(
                       l = l,
                       r = l + groupWidth,
                       t = as.vector(h[-1,]),
                       b = as.vector(h[-cBars,])
                     )
                   },
    )


    # empty chart
    opar <- par(yaxs = "i")
    on.exit(par(opar))
    plot.new()
    ylog <- getProperty('Y.Axis.Log', 'false') == 'true'
    xlim <- range(bars$l, bars$r, finite=TRUE)
    ylim <- range(bars$b, bars$t, finite=TRUE)
    plot.window(xlim, ylim, log=(if (ylog) 'y' else ''))

    # axes and grid
    groupNames <- colnames(data)
    groupCenterX <- colMeans(matrix((bars$l+bars$r)/2, nrow=cBars))
    axis(1, labels=groupNames, at=groupCenterX)
    axis(side=2)
    axis(side=4)
    switch(getProperty('Axes.Grid', 'Major'),
      None=NULL,
      Minor=grid(),
      Major=grid(),
      stop("Unsupported Axes.Grid: ", grid)
    )

    # plot bars
    barNames <- rownames(data)
    defaultColors <- rainbow(length(barNames))
    barColors <- mapply(function(name, index) getBarProperty('Bar.Color', name, defaultColors[[index]]), barNames, seq_along(barNames))
    rect(bars$l, bars$b, bars$r, bars$t, col=barColors)
    if (!is.null(conf.int) && barPlacement=='Aligned') {
      x <- (bars$l+bars$r) / 2
      w <- (bars$r-bars$l) / 4
      y0 <- as.vector(data - conf.int)
      y1 <- as.vector(data + conf.int)
      #arrows(x, y0, x, y1, angle=90, code=3, col='red')
      segments(x, y0, x, y1, col='red')
      segments(x-w, y0, x+w, y0, col='red')
      segments(x-w, y1, x+w, y1, col='red')
    }

    # plot titles
    title(main=getProperty('Graph.Title', ''),
          xlab=getProperty('X.Axis.Title'),
          ylab=getProperty('Y.Axis.Title'))


    # plot legend
    if (getProperty('Legend.Display', 'false') == 'true') {
          legendPosition  <- getProperty('Legend.Position', 'Inside')
          legendAnchoring <- getProperty('Legend.Anchoring', 'North')

          if (legendPosition != 'Inside')
              warning("Legend.Position not supported: ", legendPosition)
          legend(legendAnchoringToPosition(legendAnchoring),
                 legend=barNames,
                 pch=15,
                 col=barColors,
                 lty='blank')
    }
}