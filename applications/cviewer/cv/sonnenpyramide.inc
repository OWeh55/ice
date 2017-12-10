# basisbreite der Sonnenpyramide
spbasis=160
spb0=spbasis/2
#hoehe
sphoehe=100

# hoehe des ersten absatzes
sph1=20
spb1=(sphoehe-sph1)*spb0/sphoehe
spb1a=spb1+2

# hoehe des zweiten absatze
sph2=40
spb2=(sphoehe-sph2)*spb0/sphoehe
spb2a=spb2+2

# hoehe des dritten absatzes
sph3=60
spb3=(sphoehe-sph3)*spb0/sphoehe
spb3a=spb3+2

# hoehe des vierten absatzes = obere Plattform
sph4=80
spb4=(sphoehe-sph4)*spb0/sphoehe
spb4a=spb4+2

def sonnenpyramide
color GRAY

# erster Abschnitt (seitenteile)
polygon 
<-spb0,0,-spb0>,<-spb0,0,spb0>,<-spb1a,sph1,spb1a>,<-spb1a,sph1,-spb1a>
endpolygon
polygon 
<-spb0,0,spb0>,<spb0,0,spb0>,<spb1a,sph1,spb1a>,<-spb1a,sph1,spb1a>
endpolygon
polygon 
<spb0,0,spb0>,<spb0,0,-spb0>,<spb1a,sph1,-spb1a>,<spb1a,sph1,spb1a>
endpolygon
polygon 
<spb0,0,-spb0>,<-spb0,0,-spb0>,<-spb1a,sph1,-spb1a>,<spb1a,sph1,-spb1a>
endpolygon

# erster Absatz
polygon 
<-spb1a,sph1,-spb1a>,<-spb1a,sph1,spb1a>,<-spb1,sph1,spb1>,<-spb1,sph1,-spb1>
endpolygon
polygon 
<-spb1a,sph1,spb1a>,<spb1a,sph1,spb1a>,<spb1,sph1,spb1>,<-spb1,sph1,spb1>
endpolygon
polygon 
<spb1a,sph1,spb1a>,<spb1a,sph1,-spb1a>,<spb1,sph1,-spb1>,<spb1,sph1,spb1>
endpolygon
polygon 
<spb1a,sph1,-spb1a>,<-spb1a,sph1,-spb1a>,<-spb1,sph1,-spb1>,<spb1,sph1,-spb1>
endpolygon

# zweiter Abschnitt (seitenteile)
polygon 
<-spb1,sph1,-spb1>,<-spb1,sph1,spb1>,<-spb2a,sph2,spb2a>,<-spb2a,sph2,-spb2a>
endpolygon
polygon 
<-spb1,sph1,spb1>,<spb1,sph1,spb1>,<spb2a,sph2,spb2a>,<-spb2a,sph2,spb2a>
endpolygon
polygon 
<spb1,sph1,spb1>,<spb1,sph1,-spb1>,<spb2a,sph2,-spb2a>,<spb2a,sph2,spb2a>
endpolygon
polygon 
<spb1,sph1,-spb1>,<-spb1,sph1,-spb1>,<-spb2a,sph2,-spb2a>,<spb2a,sph2,-spb2a>
endpolygon

# zweiter Absatz
polygon 
<-spb2a,sph2,-spb2a>,<-spb2a,sph2,spb2a>,<-spb2,sph2,spb2>,<-spb2,sph2,-spb2>
endpolygon
polygon 
<-spb2a,sph2,spb2a>,<spb2a,sph2,spb2a>,<spb2,sph2,spb2>,<-spb2,sph2,spb2>
endpolygon
polygon 
<spb2a,sph2,spb2a>,<spb2a,sph2,-spb2a>,<spb2,sph2,-spb2>,<spb2,sph2,spb2>
endpolygon
polygon 
<spb2a,sph2,-spb2a>,<-spb2a,sph2,-spb2a>,<-spb2,sph2,-spb2>,<spb2,sph2,-spb2>
endpolygon

# dritter Abschnitt (seitenteile)
polygon 
<-spb2,sph2,-spb2>,<-spb2,sph2,spb2>,<-spb3a,sph3,spb3a>,<-spb3a,sph3,-spb3a>
endpolygon
polygon 
<-spb2,sph2,spb2>,<spb2,sph2,spb2>,<spb3a,sph3,spb3a>,<-spb3a,sph3,spb3a>
endpolygon
polygon 
<spb2,sph2,spb2>,<spb2,sph2,-spb2>,<spb3a,sph3,-spb3a>,<spb3a,sph3,spb3a>
endpolygon
polygon 
<spb2,sph2,-spb2>,<-spb2,sph2,-spb2>,<-spb3a,sph3,-spb3a>,<spb3a,sph3,-spb3a>
endpolygon

# dritter Absatz
polygon 
<-spb3a,sph3,-spb3a>,<-spb3a,sph3,spb3a>,<-spb3,sph3,spb3>,<-spb3,sph3,-spb3>
endpolygon
polygon 
<-spb3a,sph3,spb3a>,<spb3a,sph3,spb3a>,<spb3,sph3,spb3>,<-spb3,sph3,spb3>
endpolygon
polygon 
<spb3a,sph3,spb3a>,<spb3a,sph3,-spb3a>,<spb3,sph3,-spb3>,<spb3,sph3,spb3>
endpolygon
polygon 
<spb3a,sph3,-spb3a>,<-spb3a,sph3,-spb3a>,<-spb3,sph3,-spb3>,<spb3,sph3,-spb3>
endpolygon

# vierter Abschnitt (seitenteile)
polygon 
<-spb3,sph3,-spb3>,<-spb3,sph3,spb3>,<-spb4a,sph4,spb4a>,<-spb4a,sph4,-spb4a>
endpolygon
polygon 
<-spb3,sph3,spb3>,<spb3,sph3,spb3>,<spb4a,sph4,spb4a>,<-spb4a,sph4,spb4a>
endpolygon
polygon 
<spb3,sph3,spb3>,<spb3,sph3,-spb3>,<spb4a,sph4,-spb4a>,<spb4a,sph4,spb4a>
endpolygon
polygon 
<spb3,sph3,-spb3>,<-spb3,sph3,-spb3>,<-spb4a,sph4,-spb4a>,<spb4a,sph4,-spb4a>
endpolygon

# oberes Plateau ( (fast) eben)
polygon 
<spb4a,sph4,-spb4a>,<-spb4a,sph4,-spb4a>,<-spb4a,sph4,spb4a>,<spb4a,sph4,spb4a>
endpolygon

# treppen
color <0.3,0.3,0.3>
polygon 
<-2,sph3+0.1,spb3>,<2,sph3+0.1,spb3>,<2,sph4+0.1,spb4a>,<-2,sph4+0.1,spb4a>
endpolygon

polygon 
<-6,sph3+0.1,spb3a>,<6,sph3+0.1,spb3a>,<2,sph3+0.1,spb3>,<-2,sph3+0.1,spb3>
endpolygon

polygon 
<-9,sph2+0.1,spb2>,<-5,sph2+0.1,spb2>,<-2,sph3+0.1,spb3a>,<-6,sph3+0.1,spb3a>
endpolygon

polygon 
<5,sph2+0.1,spb2>,<9,sph2+0.1,spb2>,<6,sph3+0.1,spb3a>,<2,sph3+0.1,spb3a>
endpolygon

polygon 
<-9,sph2+0.1,spb2a>,<9,sph2+0.1,spb2a>,<9,sph2+0.1,spb2>,<-9,sph2+0.1,spb2>
endpolygon

polygon 
<-9,sph1+0.1,spb1>,<9,sph1+0.1,spb1>,<9,sph2+0.1,spb2a>,<-9,sph2+0.1,spb2a>
endpolygon

polygon 
<-9-5,sph1+0.1,spb1a>
<-9,sph1+0.1,spb1a+5>
<9,sph1+0.1,spb1a+5>
<9+5,sph1+0.1,spb1a>
<9,sph1+0.1,spb1>
<-9,sph1+0.1,spb1>
endpolygon

polygon
<9+sph1,0,spb1a+5+sph1>,<9+5+sph1,0,spb1a+sph1>,<9+5,sph1,spb1a>,<9,sph1,spb1a+5>
endpolygon

polygon
<-9,sph1,spb1a+5>,<-9-5,sph1,spb1a>,<-9-5-sph1,0,spb1a+sph1>,<-9-sph1,0,spb1a+5+sph1>
endpolygon

color GRAY
# Vorbau
polygon 
<9,sph1,spb1>,<-9,sph1,spb1>,<-9,sph1,spb1+20>, <9,sph1,spb1+20>
endpolygon

polygon 
<9,sph1,spb1+20>,<-9,sph1,spb1+20>,<-9-sph1,0,spb1+20+sph1>, <9+sph1,0,spb1+20+sph1>
endpolygon

polygon 
<-9-sph1,0,spb1>
<-9-sph1,0,spb1+20+sph1>
<-9,sph1,spb1+20>
<-9,sph1,spb1>
endpolygon

polygon 
<9,sph1,spb1>
<9,sph1,spb1+20>
<9+sph1,0,spb1+20+sph1>
<9+sph1,0,spb1>
endpolygon

enddef