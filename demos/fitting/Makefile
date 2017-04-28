MAIN=fitting

OBJECTS=$(MAIN).o

ifndef ICEDIR
ICEDIR=..
endif

LOPT := $(OPT)
COPT := $(OPT)
LIBS := -lice -ltiff -ljpeg `wx-config --libs`

ifdef SCANFW
SCANFW := 1
endif

ifdef MSYSTEM
ifeq ($(MSYSTEM),MINGW32)
#NOSCANFW := 1
NOSCANV4L := 1
VIDEOIO := 1
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

COPT := -O4 $(COPT) -std=c++11 -g -Wall  -fno-strict-aliasing 
#-ffast-math

INCLUDE := -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY := -L$(ICEDIR)/lib/$(MACHTYPE)

ifdef OPENMP
COPT := $(COPT) -fopenmp -DOPENMP
LOPT := $(LOPT) -fopenmp
endif

ifdef VIDEOIO
ifndef OSTYPE 
# win32 (?)
#ifeq ($(OSTYPE),msys)
# warum auch immer, muessen wir "ws2_32" hier anlinken ...
LIBS := $(LIBS)  -lavformat -lavcodec -lavutil -lswscale -lws2_32 -lz
#-lbz2
else
# wir sind unter Linux
LIBS := $(LIBS)  -lavformat -lavcodec -lavutil -lswscale -lz 
#-lbz2
endif
COPT := $(COPT) -DVIDEOIO
endif

ifndef NOFFTW3
LIBS := $(LIBS) -lfftw3 
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

$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

clean:
	-rm -f *.o depend *~ 

dep:
	-rm depend 
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

-include depend
