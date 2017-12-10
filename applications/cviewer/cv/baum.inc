stammdicke=0.05
wellenstaerke=0.05

fi0=48

fi1=-fi0*(1+wellenstaerke*sin(TIME*130))
fi2=-fi0*(1+wellenstaerke*sin(TIME*130+120))
fi3=-fi0*(1+wellenstaerke*sin(TIME*130-120))

def d1
scal1=0.2
color RED
cylinder NULL,Y*scal1,stammdicke*scal1
color GREEN
sphere Y*scal1,0.4*scal1
color RED
enddef

def d2
scal2=0.32
cylinder NULL,Y*scal2,stammdicke*scal2
begin
tr1=rotation(NULL,Y,120)
achse1=Z
achse2=tr1*achse1
achse3=tr1*achse2
d1 Y*scal2,fi1,achse1
d1 Y*scal2,fi2,achse2
d1 Y*scal2,fi3,achse3
end
enddef

def d3
scal3=0.51
cylinder NULL,Y*scal3,stammdicke*scal3
begin
tr1=rotation(NULL,Y,120)
achse1=Z
achse2=tr1*achse1
achse3=tr1*achse2
d2 Y*scal3,fi1,achse1
d2 Y*scal3,fi2,achse2
d2 Y*scal3,fi3,achse3
end
enddef

def d4
scal4=0.81
cylinder NULL,Y*scal4,stammdicke*scal4
begin
tr1=rotation(NULL,Y,120)
achse1=Z
achse2=tr1*achse1
achse3=tr1*achse2
d3 Y*scal4,fi1,achse1
d3 Y*scal4,fi2,achse2
d3 Y*scal4,fi3,achse3
end
enddef

def d5
scal5=1.3
cylinder NULL,Y*scal5,stammdicke*scal5
begin
tr1=rotation(NULL,Y,120)
achse1=Z
achse2=tr1*achse1
achse3=tr1*achse2
d4 Y*scal5,fi1,achse1
d4 Y*scal5,fi2,achse2
d4 Y*scal5,fi3,achse3
end
enddef

def d6
scal=2.2
cylinder NULL,Y*scal,stammdicke*scal
begin
tr1=rotation(NULL,Y,120)
achse1=Z
achse2=tr1*achse1
achse3=tr1*achse2
d5 Y*scal,fi1,achse1
d5 Y*scal,fi2,achse2
d5 Y*scal,fi3,achse3
end
enddef

def baum
color RED
d6 NULL,0,Y
enddef
