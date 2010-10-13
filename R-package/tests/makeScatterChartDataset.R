require(omnetpp)
dataset <- loadDataset(file.path(system.file('extdata', package='omnetpp'),'PureAloha1-*.sca'), add('scalar'))
d <- makeScatterChartDataset(dataset, xScalar=list(module='Aloha.server', name='duration'))
print(d)
