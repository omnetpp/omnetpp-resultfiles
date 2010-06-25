library('omnetpp')
dataset <- loadDataset(add('histogram', system.file('extdata/PureAloha1-0.sca', package='omnetpp')))
histograms <- createHistograms(dataset)
plotHistogramChart(histograms, list(Hist.Bar='Solid', Hist.Data='Count'))
