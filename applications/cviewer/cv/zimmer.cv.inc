begin
def zimmer global
color WEISS
// lange Wand
polygon
	<0,0,0>
	<0,H,0>
	<0,H,L>
	<0,0,L>
endpolygon
polygon inverse
	<B,0,0>
	<B,H,0>
	<B,H,L>
	<B,0,L>
endpolygon

polygon inverse
// links der Tuer
	<0,0,0>
	<0,H,0>
	<B1,H,0>
	<B1,0,0>
endpolygon

polygon inverse
// oberhalb der Tür
	<B1,H1,0>
	<B1,H,0>
	<B2,H,0>
	<B2,H1,0>
endpolygon
polygon inverse
	<B2,0,0>
	<B2,H,0>
	<B,H,0>
	<B,0,0>
endpolygon
if 3>4 then
polygon
// balkonseite
	<0,0,L>
	<0,H,L>
	<B,H,L>
	<B,0,L>
endpolygon
endif
polygon
// balkonseite rechter rand
	<0,0,L>
	<0,H,L>
	<BB1,H,L>
	<BB1,0,L>
endpolygon
polygon
// balkonseite oberer rand
	<0,HB2,L>
	<0,H,L>
	<B,H,L>
	<B,HB2,L>
endpolygon
polygon
// balkonseite linker rand
	<BB3,0,L>
	<BB3,H,L>
	<B,H,L>
	<B,0,L>
endpolygon
polygon
// balkonseite unterm Fenster
	<0,0,L>
	<0,HB1,L>
	<BB2,HB1,L>
	<BB2,0,L>
endpolygon
polygon 
// Decke
	<0,H,0>
	<B,H,0>
	<B,H,L>
	<0,H,L>
endpolygon
color <0.8,0.4,0.2>
polygon inverse
// Fussboden
	<0,0,0>
	<B,0,0>
	<B,0,L>
	<0,0,L>
endpolygon
enddef
end