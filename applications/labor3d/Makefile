MAIN=labor3d

OBJECTS=$(MAIN).o display3d.o dequantize.o depth.o povexport.o graynormalize.o fft3d.o selectseries.o marching.o cube.o menu.o peak.o sdr.o display3drgb.o konfokal.o

ifndef ICEDIR
ICEDIR=..
endif

AppGen=AppGen

LOPT := $(OPT) 
COPT := $(OPT) -std=c++11
LIBS := -lice -ljpeg -ltiff `wx-config --libs`

ifdef SCANFW
SCANFW := 1
endif

ifdef MSYSTEM
ifeq ($(MSYSTEM),MINGW32)
#NOSCANFW := 1
NOSCANV4L := 1
COPT := $(COPT) -D__STDC_CONSTANT_MACROS -static-libgcc -static-libstdc++
LOPT := $(LOPT) -Wl,--enable-auto-import --static -static-libgcc -static-libstdc++
LIBS := $(LIBS) -Wl,--subsystem,console -mconsole
endif
endif

ifndef MACHTYPE
MACHTYPE := some_machine
endif

#CXX=g++
#CXX=icc

COPT := -O4 $(COPT) -g -Wall  -fno-strict-aliasing 
#-ffast-math

INCLUDE := -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY := -L$(ICEDIR)/lib/$(MACHTYPE)

ifdef SCANFW
LIBS := $(LIBS) -lraw1394 -ldc1394_control 
endif

ifdef OPENMP
COPT := $(COPT) -fopenmp -DOPENMP
LOPT := $(LOPT) -fopenmp
endif

ifndef NOFFTW3
LIBS := $(LIBS) -lfftw3 #-lfftw3_omp
endif

%.o:%.c
	$(CC) $(COPT) $(INCLUDE) -c $*.c

%.o:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp

%: %.o
	$(CXX) $(COPT) $(LOPT) $(LIBRARY) -o $* $*.o $(LIBS)

%:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp
	$(CXX) $(LIBRARY) $(LOPT) -o $* $*.o $(LIBS)

%.cpp:%.ag
	$(AppGen) $*.ag $*.cpp

$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

#$(MAIN).o: $(MAIN).cpp 
#	AppGen $(MAIN).cpp $(MAIN)_out.cpp
#	$(CXX) $(COPT) $(INCLUDE) -c $(MAIN).cpp

clean:
	-rm -f *.o depend *~ 

dep:
	-rm depend 
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

-include depend
