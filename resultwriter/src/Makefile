TARGET = libresultwriter.a
OBJS = FileOutputScalarManager.o FileOutputVectorManager.o OutputFileManager.o
CXX = g++
AR = ar
RANLIB = ranlib
INCLDIR = ../include
LIBDIR = ../lib

all: $(OBJS)
	$(AR) cr $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
	mkdir -p $(LIBDIR)
	cp $(TARGET) $(LIBDIR)

.SUFFIXES: .cc

%.o: %.cc
	$(CXX) -c -I $(INCLDIR) -o $@ $<

clean:
	-rm -rf *.o $(TARGET) $(LIBDIR)/$(TARGET)


