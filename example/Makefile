#CXX=clang++

ifndef CXX
CXX=g++
endif

# main program to build
MAIN=dft_demo

# objects needed for main
OBJECTS=$(MAIN).o filefunctions.o xdisplay.o

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

INCLUDE := -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY := -L$(ICEDIR)/lib/$(MACHTYPE)

ifdef SCANFW
LIBS := $(LIBS) -lraw1394 -ldc1394_control 
endif

ifdef OPENMP
ifneq ($(CXX),clang++)
COPT := $(COPT) -fopenmp -DOPENMP
LOPT := $(LOPT) -fopenmp
endif
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

fractal: fractal.o filefunctions.o $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o fractal fractal.o filefunctions.o $(LIBS)

all: accumulatortest\
accutest\
akkutest1\
algebra\
alphatest\
background\
base_1\
base_2\
base_3\
base_4\
boxen\
calibrate\
camcalibtest\
camtest\
Cholesky\
circledetection\
circlesegtest\
circletest\
classc\
class\
classname\
classrgb\
cline\
closestpoint\
ColorArith\
color\
colorimage\
colorquantize\
compareImages\
componenttree\
congrd\
conobj\
conpgl\
conrdg\
contfeature\
contread\
contur2list\
conturPolygon\
contur_test\
conv\
convhull\
dequantize\
dequantize_int\
dfilter\
dft_demo\
dijkstra_test\
dir\
distancetest\
distortiontest\
dobdemo\
ecfit\
eigentest\
equsys\
erode\
erodil\
errortest\
f3d\
fcompare\
fd\
fdgen\
fft_bench\
fft\
fileseltest\
filterdemo\
fitfunk\
fit_moments\
flip\
fmittel\
four1\
four2\
four\
fourier1d\
fourier2d\
fouriertest_1\
fouriertest2d\
fouriertest\
fouritime\
fourtime\
freader\
ftest\
ftestm1\
ftestm2\
ftestm\
functions\
ganglien\
gaussfilter\
gausstest\
geo3dtest\
geo_obj\
getval\
GradImg\
graf\
grafik\
graf_obj\
grauk\
haar\
halb\
hartt\
hist\
histogram\
hshift\
hsiConvert\
hungarian\
idif\
image1\
imgread\
imgsvd\
input\
interpol\
intersection\
jtest\
kdindextest\
kdtest\
klttest\
korrel\
lacunarity\
LaplaceImg\
laufzeit\
linequ_bench\
linequ\
linetest\
LinHist\
lmfunctortest\
lmsolvetest1\
lmsolvetest\
lmstereo\
lmtest1\
lmtest\
lnorm\
localseg\
localsegobj\
los\
lseg\
LSIImg\
LSIImgH\
LsiInverse\
ludcmptest\
match_test\
matinv\
matrixbench\
MatrixInverse\
matrixtest\
MedianImg\
menu\
MexicanImg\
mintreetest\
mkfilm\
momentfit\
momenttest\
morphology\
mouse\
objectmatcher\
ofilter\
overlaycheck\
paint\
parith\
peak1d\
picfile\
picio\
plsort\
polygonfit\
polygontest\
qrtest\
radon\
range\
rankimg\
readimg\
readpbm\
reducepoly\
region1\
region2\
region\
regiongrow\
regions\
RenormImg\
resampling\
rgb2h\
ridge\
selectcontur\
selfcalib\
shadingtest\
skdtest\
skelett_test\
smear\
sort\
splitandmerge\
ssdr\
statistik\
stringtest\
subimg\
term\
test\
text\
thinning\
trafo\
transformImage\
transimg\
triginter\
trtest1\
trtest\
trtest_move\
tst\
valref\
vectordistancetest\
vectortest\
vis1\
visual\
visualx\
vreaderchachetest\
yuvConvert\
capture_v4l2\
circledetection\
classtest\
convinter\
convolutionimg\
delaunay\
detectShift\
dft_demo\
dftdemo\
EqsBench\
filterbackprojection\
fractal\
hmittel\
invconvolution\
kdmosaic\
match2d\
mittel\
momentnormtest\
power\
sdrClassTest\
sdr_local\
sdrtest\
showFilter\
siemens\
FourierTrafoT\
ConvolutionT\
InvConvolutionT\
FourierTrafo2DT


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
