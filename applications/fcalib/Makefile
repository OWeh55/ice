ifndef CXX
#CXX=clang++
#CXX=g++-5
CXX=g++
endif

# main program to build
MAIN1=undistort
MAIN2=fcalib

# objects needed for main
OBJECTS1=$(MAIN1).o FileName.o exifsig.o filelist.o
OBJECTS2=$(MAIN2).o fc_marker.o fc_poly.o fc_references.o filelist.o exifsig.o

ifndef ICEDIR
ICEDIR=../..
endif

LOPT :=
COPT := -std=c++11
LIBS := -lice -ljpeg -ltiff `wx-config --libs`
# -lblas

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

COPT := $(COPT) -O4 -g -fno-strict-aliasing -Wall -Wno-unused-local-typedefs
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
LIBS := $(LIBS) -lfftw3 
else
COPT := $(COPT) -DNOFFTW3
endif

# add OPT to allow overriding other flags
COPT := $(COPT) $(OPT)

# command for beautifier
ASTYLE := astyle
# format options for beautifier
ASTYLE_OPT = --style=gnu --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type

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
	AppGen $*.ag $*.cpp

all: $(MAIN1) $(MAIN2) patterngen

pattern.inc:	patterngen
	patterngen -c pattern.inc

$(MAIN1): $(OBJECTS1) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o $(MAIN1) $(OBJECTS1) $(LIBS)

$(MAIN2): $(OBJECTS2) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o $(MAIN2) $(OBJECTS2) $(LIBS)

clean_files:
	-rm -f *.o depend *~ *.orig
	find . -type f -executable -delete

dep:
	-rm -f depend 
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

clean:	clean_files dep

format:
	$(ASTYLE) $(ASTYLE_OPT) *.cpp *.h *.ag

-include depend
