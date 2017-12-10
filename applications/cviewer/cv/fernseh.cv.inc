FTB=0.84*FAK
FTH=0.74*FAK
FTT=0.42*FAK

begin
def fernseher
box NULL,<0.39*FAK,0.36*FAK,0.08*FAK>
enddef

def fernsehtisch global
color WHITE*0.9 // nicht ganz weiss
box NULL,<FTB,FTH,FTT>
color WHITE
fernseher <0.1*FAK,FTH,0.3*FAK>,0,Y
enddef
end
