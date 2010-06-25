library('omnetpp')
dataset <- loadDataset(add('vector', system.file('extdata/PureAloha1-0.vec', package='omnetpp'), select='name("channel utilization")'))
vectors <- merge(dataset$files, dataset$vectors)[,c('ospath', 'vectorid')]

ops <- list(compare(0,1,-1,2),
            crop(0, 1),
            difference(),
            diffquot(),
            divideBy(2),
            divtime(),
            expression('yprev'),
            integrate('linear'),
            lineartrend(1),
            mean(),
            modulo(2),
            movingavg(0.3),
            multiplyBy(2),
            removerepeats(),
            slidingwinavg(10),
            sum(),
            timeavg('linear'),
            timediff(),
            timeshift(dt=0.1),
            timetoserial(),
            timewinavg(10),
            winavg(10))            

for (op in ops) {
  print(loadVectors(vectors, apply(op)))
  print(loadVectors(vectors, compute(op)))
}
