MAIN=tv
OBJECTS=$(MAIN).o \
	pbmio.o\
	matrix3.o \
	numbase.o \
	read.o \
	stringtools.o \
	surface.o \
	trafo.o \
	vector3.o 

CXX=g++
CFLAGS= $(OPT) -std=c++11 -g -Wall

INCLUDE=
LIBRARY=
LIBS= -lm  -ljpeg -lX11 -lGL -lglut -lGLU -lpthread

ASTYLE := astyle
ASTYLE_OPT = --style=gnu --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type

#BISONOPT=-d -v -t 
BISONOPT=-d -v

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $*.c

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp

%:%.o
	$(CXX) $(CFLAGS) $(LIBRARY) -o $* $*.o $(LIBS)

%.cpp:%.ag
	AppGen $*.ag $*.cpp

$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a Makefile | dep
	$(CXX) $(OPT) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

.PHONY:	format clean dep

format:
	$(ASTYLE) $(ASTYLE_OPT) *.cpp *.h

clean:
	-rm -f *.o depend *~ 
	-rm *.orig

dep:
	-rm depend
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

# all:   $(MAIN) | format


-include depend
