library('omnetpp')
dataset <- loadDataset(file.path(system.file('extdata', package='omnetpp'),'PureAlohaExperiment-4*.sca'),
                       add('scalar', 'module("Aloha.server")'))
d <- createBarChartDataset(dataset, rows=c('measurement'), columns=c('name'))
par(mfrow=c(2,2))
plotBarChart(d, list(Legend.Display='true', Legend.Anchoring='NorthWest'))
plotBarChart(d, list(Bar.Placement='Stacked'))
plotBarChart(d, list(Bar.Placement='Overlapped'))
plotBarChart(d, list(Bar.Placement='Infront'))
