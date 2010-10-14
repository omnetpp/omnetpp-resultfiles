require(omnetpp)
aloha <- loadDataset(c(file.path(system.file('extdata',package='omnetpp'),'PureAloha*.sca'),
                       file.path(system.file('extdata',package='omnetpp'),'PureAloha*.vec')))
