library('omnetpp')
dataset <- loadDataset(system.file('extdata/PureAloha1-0.sca', package='omnetpp'), add('histogram'))
print(createHistograms(dataset))
