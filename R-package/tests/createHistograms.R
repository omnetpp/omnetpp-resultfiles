library('omnetpp')
dataset <- loadDataset(add('histogram', system.file('extdata/PureAloha1-0.sca', package='omnetpp')))
print(createHistograms(dataset))
