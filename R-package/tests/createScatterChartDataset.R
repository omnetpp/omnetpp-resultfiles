library('omnetpp')
dataset <- loadDataset(add('scalar', file.path(system.file('extdata', package='omnetpp'),'PureAloha1-*.sca')))
d <- createScatterChartDataset(dataset, xModule='Aloha.server', xName='duration')
print(d)
