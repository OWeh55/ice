MAIN=omatch

OBJECTS=$(MAIN).o

#CXX=g++
#CXX=icc

CFLAGS= -O4 $(OPT) -g -Wall -fno-strict-aliasing
#CFLAGS= $(OPT) -Wall 

INCLUDE= -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY= -L$(ICEDIR)/lib/$(MACHTYPE)
LIBS= -lice `wx-config --libs` -lfftw3 -ltiff -ljpeg
#-lraw1394 -ldc1394_control 
#--lmsvcp60


%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $*.c

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp

%: %.o
	$(CXX) $(CFLAGS) $(LIBRARY) -o $* $*.o $(LIBS)

%:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp
	$(CXX) $(CFLAGS) $(LIBRARY) -o $* $*.o $(LIBS)


$(MAIN): $(OBJECTS) $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(OPT) $(LIBRARY) -o $(MAIN) $(OBJECTS) $(LIBS)

all:algebra\
alphatest\
base_1\
base_2\
base_3\
base_4\
base_5\
bfile\
boxen\
camtest\
chkdisplay\
class\
cline\
color\
complneu\
congrd\
conpgl\
conrdg\
contur_test\
conv\
convert\
dir\
equsys\
erode\
erodil\
fileseltest\
filt2\
fitfunk\
fit_moments\
four1\
four\
ftest\
gausstest\
geo3dtest\
geo_obj\
graf\
grafik\
grauk\
haar\
halb\
hartt\
hfile\
hist\
hungarian\
image1\
image2\
imgsvd\
input\
jtest\
laufzeit\
lineqn\
linequ_bench\
linequ\
lmfunctortest\
lmstereo\
lmtest\
los\
ludcmptest\
matinv\
matrix\
menu\
momenttest\
mouse\
overlaycheck\
paint\
parith\
picio\
plsort\
polygn\
qrtest\
region1\
region2\
region\
regiongrow\
rgbimg\
ridge\
scan\
seglist\
sort\
stringtest\
system\
term\
thinning\
trafo\
transimg\
trtest\
tst\
vectortest\
vis1\
vis2\
visual\
visualx\
wsel\
wstt

clean:
	-rm -f *.o depend *~ 

dep:
	-rm depend 
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

-include depend


