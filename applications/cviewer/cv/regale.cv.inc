// Regale 

HR=2*FAK // Regalhöhe
BR=0.8*FAK // Regalbreite (breit)
BR1=0.4*FAK // Regalbreite (schmal)
TR=0.4*FAK // Regaltiefe

begin

DRB=0.02*FAK // Brettdicke

def regalbrett
box <DELTA,0,DELTA>, <BR-DELTA,DRB,TR-DELTA>
enddef

def seitenwand
box NULL,<DRB,HR,TR>
enddef

def regal global
color YELLOW
seitenwand NULL,0,Y
seitenwand <BR-DRB,0,0>,0,Y
for i=0 to 5 
 pos=Y*(0.38*FAK*i+0.05*FAK)
 regalbrett pos,0,Y
endfor
enddef

// schmales regal

def regalbrett1
box <DELTA,0,DELTA>, <BR1-DELTA,DRB,TR-DELTA>
enddef

def regal1 global
color YELLOW
seitenwand NULL,0,Y
seitenwand <BR1-DRB,0,0>,0,Y
for i=0 to 5 
 pos=Y*(0.38*FAK*i+0.05*FAK)
 regalbrett1 pos,0,Y
endfor
enddef
end