MAIN=cviewer
OBJECTS=$(MAIN).o \
	arith.o \
	box.o \
	cylinder.o \
	function.o \
	hf.o \
	lexan.o \
	matrix3.o \
	multiple.o \
	numbase.o \
	object.o \
	objekte.o \
	operatoren.o \
	parser.o \
	parserb.o \
	pbmio.o \
	point.o \
	polygon.o \
	sphere.o \
	stringtools.o \
	surface.o \
	tools.o \
	trafo.o \
	triangle.o \
	variablen.o \
	vector3.o 

CXX=g++
CFLAGS= $(OPT) -g -Wall

INCLUDE=
LIBRARY=
LIBS= -lm  -ljpeg -lX11 -lGL -lglut -lGLU -lpthread

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

lexan.cpp:  lexan.l parserb.cpp Makefile
	# flex -d -i -t -8 -f lexan.l > lexan.cpp
	flex -i -t -8 -f lexan.l > lexan.cpp

parserb.cpp:  parserb.y Makefile
	bison $(BISONOPT) parserb.y -o parserb.cpp
#	mv parserb.tab.c parserb.cpp
#	mv parserb.tab.h parserb.h

.PHONY:	format clean dep

format:
	astyle *.cpp *.h

clean:
	-rm -f *.o depend *~ 
	-rm parserb.cpp parserb.hpp
	-rm lexan.cpp
	-rm *.orig

dep:
	-rm depend
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

# all:   $(MAIN) | format


-include depend
