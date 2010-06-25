require(omnetpp)

scalarFiles <- file.path(system.file('extdata', package='omnetpp'), 'PureAlohaExperiment-*.sca')
vectorFiles <- c(system.file('extdata', 'OneFifo-0.vec', package='omnetpp'),
                 system.file('extdata', 'TokenRing1-0.vec', package='omnetpp'))

#
# Scatter chart from OMNeT++ samples/resultfiles/PureAlohaExperiment.anf 
#
# TODO iso lines not yet implemented
dataset <- loadDataset(
             add(type='scalar', files=scalarFiles, select='module(Aloha.server) AND name("channel utilization")'),
             add(type='scalar', files=scalarFiles, select='name(mean)'))
xydata <- createScatterChartDataset(dataset, xModule='.', xName='mean', averageReplications=TRUE)
plotLineChart(xydata, list(X.Axis.Title='Mean packet interarrival time', Y.Axis.Title='Utilization'))


#
# Histogram from OMNeT++ samples/resultfiles/PureAlohaExperiment.anf 
#
dataset <- loadDataset(
             add(type = 'histogram',
                 files = file.path(system.file('extdata', package='omnetpp'), 'PureAlohaExperiment-*.sca'),
                 select= 'module(Aloha.server) AND name("collision multiplicity") AND run(PureAlohaExperiment-1-*)'))
histograms <- createHistograms(dataset)
plotHistogramChart(histograms, list(X.Axis.Title='number of colliding packets', Y.Axis.Title='number of times occured'))

#
# Plotting vectors.
#
dataset <- loadDataset(add(files=vectorFiles, select='name("Queue length (packets)")'))
data <- loadVectors(dataset)
plotLineChart(split(data$vectordata, data$vectordata$vector_key), list(X.Axis.Title='simulation time', Y.Axis.Title='queue length'))

#
# Some bar charts.
#
dataset <- loadDataset(add('scalar', scalarFiles, 'module("Aloha.server") AND file(*PureAlohaExperiment-4*.sca)'))
d <- createBarChartDataset(dataset, rows=c('measurement'), columns=c('name'))
par(mfrow=c(2,2))
plotBarChart(d, list(Legend.Display='true', Legend.Anchoring='NorthWest'))
plotBarChart(d, list(Bar.Placement='Stacked'))
plotBarChart(d, list(Bar.Placement='Overlapped'))
plotBarChart(d, list(Bar.Placement='Infront'))
