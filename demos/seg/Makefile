CXX=g++

ICEDIR := $(HOME)/ice

OBJDIR := $(MACHTYPE)
DEPDIR := $(MACHTYPE)
LIBDIR := $(ICEDIR)/lib/$(MACHTYPE)

INCLUDE := -I $(ICEDIR)/src
LIBRARY := -L $(LIBDIR) `wx-config --libs` -lice -lfftw3 -lraw1394 -ldc1394_control

COPT := $(OPT) -g -Wall `wx-config --cxxflags` -fno-strict-aliasing 


SRC := $(wildcard *.cpp)

OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))
DEP := $(patsubst %.cpp,$(DEPDIR)/%.d,$(notdir $(SRC)))

$(OBJDIR)/%.o:	%.cpp
	$(CXX) $(INCLUDE) $(COPT) -c $< -o $@

# Standard-Ziel
all: preparation lseg

lseg: $(OBJDIR)/lseg.o 
# $(OBJDIR)/matchimg.o
#	$(CXX) $(OPT) $(OBJDIR)/smatch.o $(OBJDIR)/matchimg.o $(LIBRARY) -o $@
	$(CXX) $(OPT) $(OBJDIR)/lseg.o $(LIBRARY) -o $@

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
