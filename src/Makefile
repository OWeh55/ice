##########################################
#     		ICE Makefile              #
###########################################
#
# If you want to exclude visualization 
# routines from the library, just define
# the variable NOVISUAL with :
#		make NOVISUAL=1
# This is the only way to use the ICE
# library without wxWidgets
#
# use environment variable OPT for special
# compiler options (e.g. -fPIC )
###########################################

#NOFFTW3=1
#CXX := g++
CXX=g++
#CXX=clang++

SUBDIRS := devices ocr

COPT := -std=c++11 -D__STDC_CONSTANT_MACROS=1

ifdef ANALYSE
COPT := $(COPT) -Wall -Wunused-value -Wunused-variable -Wunused-result -Wzero-as-null-pointer-constant \
-Wextra -Wno-unused-parameter -Wchar-subscripts -Wundef -Wcast-align -Wwrite-strings 
endif

ifdef OPENMP
ifneq ($(CXX),clang++)
COPT := $(COPT) -DOPENMP -fopenmp
endif
endif

ifdef NOFFTW3
COPT := $(COPT) -DNOFFTW3
endif

# we use MACHTYPE as subdirectory name to allow parallel 
#  build on different architektures
# we need to make sure, there is an definition of MACHTYPE
ifndef MACHTYPE
MACHTYPE := some_machine
endif

OBJDIR := $(MACHTYPE)/obj
DEPDIR := $(MACHTYPE)/deps

# visualization related parts separated 
OBJVISDIR := $(MACHTYPE)/objvis
DEPVISDIR := $(MACHTYPE)/depsvis
VISDIR := visual

# where to put library
LIBDIR := ../lib/$(MACHTYPE)

# command for beautifier
ASTYLE := astyle
# format options for beautifier
ASTYLE_OPT = --style=gnu --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type
#ASTYLE_OPT = --style=1tbs --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type 

ALLDIRS := . $(SUBDIRS) $(VISDIR)
VPATH := $(SUBDIRS)

ifdef NOVISUAL
COPT := $(COPT) -DNOVISUAL
endif

# optimize, but allow overriding with OPT
# debug symbols
# "all" warnings, but not unused-local-typedefs (since wxWidgets uses this)
COPT := $(COPT) -O4 -g -Wall -Wno-unused-local-typedefs

# now allow OPT to override previous flags
COPT := $(COPT) $(OPT)

# -Wzero-as-null-pointer-constant \
# -Wextra -Wno-unused-parameter -Wchar-subscripts -Wundef -Wcast-align -Wwrite-strings 
# -Werror
# -Wshadow
# -Wsign-compare -Wunused -Wuninitialized -Wpointer-arith 
# -Wredundant-decls -Wmissing-declarations 
# -Wlong-long -Werror -Wno-error=unused-parameter
# -Wconversion

# visualization related parts need extra flags 
COPTVIS := $(COPT) `wx-config --cxxflags` -fno-strict-aliasing

INCLUDE := -I . 

INCLUDEVIS := $(INCLUDE)

SRC := $(wildcard *.cpp)
SRCVIS := $(wildcard $(VISDIR)/*.cpp)

#HDR := $(wildcard *.h)
#HDROK := $(patsubst %.h,%.hok,$(notdir $(HDR)))

include $(patsubst %, %/module.mk, $(SUBDIRS))

OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))
DEP := $(patsubst %.cpp,$(DEPDIR)/%.d,$(notdir $(SRC)))

OBJVIS := $(patsubst %.cpp,$(OBJVISDIR)/%.o,$(notdir $(SRCVIS)))
DEPVIS := $(patsubst %.cpp,$(DEPVISDIR)/%.d,$(notdir $(SRCVIS)))

$(OBJDIR)/%.o:	%.cpp
	$(CXX) $(INCLUDE) $(COPT) -c $< -o $@

$(OBJVISDIR)/%.o: $(VISDIR)/%.cpp
	$(CXX) $(INCLUDEVIS) $(COPTVIS) -c $< -o $@

# standard target
ifndef NOVISUAL
all: preparation buildlib
	@echo "The library was built successfully."
else
all: preparation buildlibnovis
	@echo "The library was built successfully WITHOUT visualization."
endif

preparation:
	mkdir -p $(LIBDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJVISDIR)
	mkdir -p $(DEPDIR)
	mkdir -p $(DEPVISDIR)

buildlib: $(HDROK) $(OBJ) $(OBJVIS)
	-rm $(LIBDIR)/libice.a
	ar ruvs $(LIBDIR)/libice.a $(OBJ) $(OBJVIS)
	ranlib $(LIBDIR)/libice.a

buildlibnovis: $(OBJ)
	ar ruvs $(LIBDIR)/libice.a $(OBJ)
	ranlib $(LIBDIR)/libice.a

$(DEPDIR)/%.d: %.cpp
	mkdir -p $(DEPDIR)
	@echo "** building dependencies of $< (non-visual)"
	./depend.sh `dirname $*.cpp` $(OBJDIR) $(COPT) $(INCLUDE) $< > $@

-include $(DEP)

ifndef NOVISUAL

$(DEPVISDIR)/%.d: $(VISDIR)/%.cpp
	mkdir -p $(DEPVISDIR)
	@echo "** building dependencies of $< (visual)"
	./depend.sh `dirname $(VISDIR)/$*.cpp` $(OBJVISDIR) $(COPTVIS) $(INCLUDEVIS) $< > $@

-include $(DEPVIS)

endif

format:
	$(ASTYLE) $(ASTYLE_OPT) $(patsubst %, %/*.cpp, $(ALLDIRS))
	$(ASTYLE) $(ASTYLE_OPT) $(patsubst %, %/*.h, $(ALLDIRS))
	$(ASTYLE) $(ASTYLE_OPT) *.inc

dep:
	# "make dep ist nicht mehr erforderlich!"

clean:
	-rm -f $(OBJDIR)/*.o
	-rm -f $(DEPDIR)/*.d
	-rm -f $(OBJVISDIR)/*.o
	-rm -f $(DEPVISDIR)/*.d
	-rm $(patsubst %, %/*.orig, $(ALLDIRS))	
	-rm $(patsubst %, %/*~, $(ALLDIRS))	
