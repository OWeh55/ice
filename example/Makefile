ifndef CXX
#CXX=clang++
#CXX=g++-5
CXX=g++
endif

# main program to build
MAIN=vis2

# objects needed for main
OBJECTS=$(MAIN).o filefunctions.o

ifndef ICEDIR
ICEDIR=..
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

INCLUDE := -I/c/msys/lib/fftw-3.3.8/api/ -I/c/msys/lib/jpeg-6b/ -/c/msys/lib/tiff-4.0.4/libtiff/ -I$(ICEDIR)/src `wx-config --cxxflags` -I.
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

$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

all: algebra\
alphatest\
base_1\
base_2\
base_3\
base_4\
boxen\
camtest\
chkdisplay\
class\
classname\
classrgb\
classtest\
cline\
closestpoint\
color\
ColorArith\
congrd\
conpgl\
conrdg\
contread\
contur_test\
conv\
dir\
distortiontest\
equsys\
erode\
erodil\
fcompare\
fft\
fft_bench\
fileseltest\
filt0\
filt2\
fitfunk\
fit_moments\
fmittel\
four\
four1\
fouritime\
fourtime\
freader\
ftest\
ftestm\
ftestm1\
ftestm2\
ganglien\
gausstest\
geo3dtest\
geo_obj\
getval\
GradImg\
graf\
grafik\
grauk\
haar\
halb\
hartt\
hist\
histogram\
hsiConvert\
hungarian\
idif\
image1\
imgsvd\
input\
jtest\
korrel\
LaplaceImg\
laufzeit\
linequ\
linequ_bench\
LinHist\
lmfunctortest\
lmstereo\
lmtest\
lnorm\
localseg\
localsegobj\
los\
LSIImg\
ludcmptest\
matinv\
matrixbench\
matrixtest\
MedianImg\
menu\
MexicanImg\
mittel\
momenttest\
mouse\
overlaycheck\
paint\
parith\
picfile\
picio\
plsort\
polygontest\
qrtest\
readimg\
reducepoly\
region\
region1\
region2\
regiongrow\
RenormImg\
rgb2h\
ridge\
selfcalib\
smear\
sort\
statistik\
stringtest\
subimg\
term\
thinning\
trafo\
transimg\
trtest\
trtest1\
tst\
valref\
vectortest\
vis1\
vis2\
visual\
visualx\
yuvConvert

#LSIImgH\
#subImage\

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
