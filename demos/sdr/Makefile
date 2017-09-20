CXX=g++

ICEDIR := $(HOME)/ice

OBJDIR := $(MACHTYPE)
DEPDIR := $(MACHTYPE)
LIBDIR := $(ICEDIR)/lib/$(MACHTYPE)

INCLUDE := -I $(ICEDIR)/src
LIBRARY := -L $(LIBDIR) `wx-config --libs` -lice -lfftw3

COPT := $(OPT) -std=c++11 -g -Wall `wx-config --cxxflags` -fno-strict-aliasing

SRC := $(wildcard *.cpp)

OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))
DEP := $(patsubst %.cpp,$(DEPDIR)/%.d,$(notdir $(SRC)))

$(OBJDIR)/%.o:	%.cpp
	$(CXX) $(INCLUDE) $(COPT) -c $< -o $@

# Standard-Ziel
all: preparation smatch

smatch: $(OBJDIR)/smatch.o $(OBJDIR)/matchimg.o
	$(CXX) $(OPT) $(OBJDIR)/smatch.o $(OBJDIR)/matchimg.o $(LIBRARY) -o $@

mkmuster: $(OBJDIR)/mkmuster.o
	$(CXX) $(OPT) $(OBJDIR)/mkmuster.o $(LIBRARY) -o $@

muster1.dat: muster1
	muster1

muster.ps: mkmuster muster1.dat
	mkmuster muster1.dat muster.ps

preparation:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(DEPDIR)/%.d: %.cpp
	mkdir -p $(DEPDIR)
	./depend.sh `dirname $*.cpp` $(OBJDIR) $(COPT) $(INCLUDE) $< > $@

-include $(DEP)

dep:
#
# "make dep ist nicht mehr erforderlich!"
#

clean:
	-rm $(OBJDIR)/*.o
	-rm $(DEPDIR)/*.d
