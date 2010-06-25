library('omnetpp')
d <- loadDataset(
       add(files=file.path(system.file('extdata', package='omnetpp'),'PureAloha1-*.sca')),
       add('vector', system.file('extdata/PureAloha1-0.vec', package='omnetpp')))

print(d)
