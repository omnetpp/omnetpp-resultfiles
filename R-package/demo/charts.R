require(omnetpp)

scalarFiles <- file.path(system.file('extdata', package='omnetpp'), 'PureAlohaExperiment-*.sca')
vectorFiles <- c(system.file('extdata', 'OneFifo-0.vec', package='omnetpp'),
                 system.file('extdata', 'TokenRing1-0.vec', package='omnetpp'))

#
# Scatter chart from OMNeT++ samples/resultfiles/PureAlohaExperiment.anf
#
dataset <- loadDataset(scalarFiles,
             add(type='scalar', select='module(Aloha.server) AND name("channel utilization")'),
             add(type='scalar', select='name(mean)'))
xydata <- makeScatterChartDataset(dataset, xModule='.', xName='mean', isoAttrs='numHosts', averageReplications=TRUE)
plotLineChart(xydata, list(X.Axis.Title='Mean packet interarrival time', Y.Axis.Title='Utilization'))


#
# Histogram from OMNeT++ samples/resultfiles/PureAlohaExperiment.anf
#
dataset <- loadDataset(file.path(system.file('extdata', package='omnetpp'), 'PureAlohaExperiment-*.sca'),
             add('statistic', 'module(Aloha.server) AND name("collision multiplicity") AND run(PureAlohaExperiment-1-*)'))
histograms <- makeHistograms(dataset)
plotHistogramChart(histograms, list(X.Axis.Title='number of colliding packets', Y.Axis.Title='number of times occured'))

#
# Plotting vectors.
#
dataset <- loadDataset(vectorFiles, add(select='name("Queue length (packets)")'))
data <- loadVectors(dataset$vectors)
plotLineChart(split(data$vectordata, data$vectordata$resultkey), list(X.Axis.Title='simulation time', Y.Axis.Title='queue length'))

#
# Some bar charts.
#
dataset <- loadDataset(scalarFiles, add('scalar', 'module("Aloha.server") AND file(*PureAlohaExperiment-4*.sca)'))
print(dataset)
d <- makeBarChartDataset(dataset, rows=c('measurement'), columns=c('name'))
ci <- makeBarChartDataset(dataset, rows=c('measurement'), columns=c('name'), conf.int(0.95) )
par(mfrow=c(2,2))
plotBarChart(d, list(Legend.Display='true', Legend.Anchoring='NorthWest'), conf.int=ci)
plotBarChart(d, list(Bar.Placement='Stacked'))
plotBarChart(d, list(Bar.Placement='Overlapped'))
plotBarChart(d, list(Bar.Placement='Infront'))
