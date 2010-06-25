require(omnetpp)
dataset <- loadDataset(file.path(system.file('extdata', package='omnetpp'),'PureAloha1-*.sca'), add('scalar'))
d <- createScatterChartDataset(dataset, xModule='Aloha.server', xName='duration')
print(d)
