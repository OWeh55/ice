MAIN=sdp

ICEDIR=../..

OBJECTS=$(MAIN).o string.o makefilter.o gdtype.o genimage.o filterfactory.o\
f_filter.o f_convert.o f_delaunay.o f_pointset.o f_draw.o f_info.o \
f_ifilter.o compile.o file.o parser.o f_contur.o f_region.o f_mask.o \
f_histogram.o f_dial.o f_display.o f_eval.o f_fourier.o stackmachine.o

#CXX=g++
#CXX=icc

APPGEN=AppGen

CFLAGS= -O4 $(OPT) -g -Wall -fno-strict-aliasing -std=c++11  

#CFLAGS= $(OPT) -Wall 

INCLUDE= -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY= -L$(ICEDIR)/lib/$(MACHTYPE)
LIBS= -lice -ljpeg -ltiff `wx-config --libs` -lfftw3 
#-lraw1394 -ldc1394_control 
#--lmsvcp60

ifdef OPENMP
CFLAGS := $(CFLAGS) -fopenmp -DOPENMP
endif


%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $*.c

%.cpp:%.ag
	$(APPGEN) $*.ag $*.cpp

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp

%: %.o
	$(CXX) $(CFLAGS) $(LIBRARY) -o $* $*.o $(LIBS)

%:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp
	$(CXX) $(CFLAGS) $(LIBRARY) -o $* $*.o $(LIBS)

$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a 
	$(CXX) $(CFLAGS) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

clean_files:
	-rm -f *.o depend *~ *.orig sdp.cpp

clean:	clean_files dep

format:
	astyle *.h *.cpp *.ag

dep:
	-rm -f depend 
	touch depend
	$(APPGEN) sdp.ag sdp.cpp
	$(CXX) $(CFLAGS) -M $(COPT) $(INCLUDE) *.cpp >> depend

all: format clean dep $(MAIN)

parstest: parstest.o parser.o
	$(CXX)  parstest.o parser.o -o parstest

smtest: smtest.o stackmachine.o parser.o
	$(CXX)  smtest.o stackmachine.o parser.o -o smtest

filetest: filetest.o file.o
	$(CXX)  filetest.o file.o -o filetest

-include depend
