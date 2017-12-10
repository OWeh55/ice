TH=0.8*FAK
TB=1.0*FAK
TT=0.6*FAK

RT1=0.16*FAK
RTH1=0.05*FAK
RTH2=0.46*FAK
RTD=0.02*FAK
RTR=0.02*FAK

SB=0.56*FAK
SRAD=0.016*FAK
SH0=SRAD
SH1=0.35*FAK+SRAD
SH2=0.65*FAK+SRAD
ST=0.6*FAK


begin
TR=0.03*FAK
TD=0.05*FAK

def tisch global
color BLACK
cylinder <0+TD,0,TD>,<0+TD,TH-DELTA,TD>,TR
cylinder <0+TD,0,TT-TD>,<0+TD,TH-DELTA,TT-TD>,TR
cylinder <TB-TD,0,TD>,<TB-TD,TH-DELTA,TD>,TR
cylinder <TB-TD,0,TT-TD>,<TB-TD,TH-DELTA,TT-TD>,TR
color YELLOW
box <0,TH,0>,<TB,TH+0.02*FAK,TT>
enddef

def rundertisch global
color SCHWARZ
cylinder <RT1,RTH1,RT1>,<RT1,RTH2,RT1>,RTR
cylinder <-RT1,RTH1,RT1>,<-RT1,RTH2,RT1>,RTR
cylinder <-RT1,RTH1,-RT1>,<-RT1,RTH2,-RT1>,RTR
cylinder <RT1,RTH1,-RT1>,<RT1,RTH2,-RT1>,RTR
cylinder <0,RTH1,0>,<0,RTH1+RTD,0>,RT1*1.5
cylinder <0,RTH2,0>,<0,RTH2+RTD,0>,RT1*1.5
enddef

RCB=0.44*FAK
RCT=0.57*FAK
RCH=0.47*FAK
RCH1=0.05*FAK
RCD=0.02*FAK

def rollcontainer global
color BLACK
box <0,RCH1,0>,<RCB,RCH+RCH1,RCT>
color YELLOW
for i=0 to 3
box <RCD,RCH1+i*RCH/4+RCD,0+0.01>,<RCB-RCD,RCH1+(i+1)*RCH/4-RCD,RCT+0.01>
endfor
enddef

def sessel global
color SILVER
pos000=<0,SH0,0>
pos100=<SB,SH0,0>
pos001=<0,SH0,ST>
pos101=<SB,SH0,ST>
pos011=<0,SH1,ST>
pos111=<SB,SH1,ST>
pos010=<0,SH1,0>
pos110=<SB,SH1,0>
pos020=<0,SH2,0>
pos120=<SB,SH2,0>
pos021=<0,SH2,ST>
pos121=<SB,SH2,ST>
sphere pos000,SRAD
cylinder pos000,pos001,SRAD
sphere pos001,SRAD
cylinder pos001,pos021,SRAD
sphere pos021,SRAD
cylinder pos021,pos020,SRAD
sphere pos020,SRAD
cylinder pos020,pos120,SRAD
sphere pos120,SRAD
cylinder pos120,pos121,SRAD
sphere pos121,SRAD
cylinder pos121,pos101,SRAD
sphere pos101,SRAD
cylinder pos101,pos100,SRAD
sphere pos100,SRAD
cylinder pos100,pos000,SRAD

color RED
box pos010,pos111
box pos010,pos120
enddef

end