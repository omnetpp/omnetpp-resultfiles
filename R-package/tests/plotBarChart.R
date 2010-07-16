require(omnetpp)
dataset <- loadDataset(file.path(system.file('extdata', package='omnetpp'),'PureAlohaExperiment-4*.sca'),
                       add('scalar', 'module("Aloha.server")'))
d <- makeBarChartDataset(dataset, rows=c('measurement'), columns=c('name'))
par(mfrow=c(2,2))
plotBarChart(d, Legend.Display='true', Legend.Anchoring='NorthWest')
plotBarChart(d, Bar.Placement='Stacked')
plotBarChart(d, Bar.Placement='Overlapped')
plotBarChart(d, Bar.Placement='Infront')
