IMAGES=		ellipse.eps \
		ellipseseg.eps \
		circle.eps \
		circleseg.eps \
		contur1.eps \
		contur2.eps \
		contur3.eps \
		freeman.eps

ICESECT=contributions.tex einleit.tex bilder.tex einaus.tex bearb.tex data.tex \
	fitting.tex filter.tex geoobj.tex grafik.tex graph.tex klassif.tex \
	konturen.tex matrizen.tex momente.tex numerik.tex register.tex \
	statist.tex signal.tex style.tex trafo.tex camera.tex tools.tex \
	visual.tex obsolet.tex functions.tex segment.tex

ROBSECT=	intro.tex \
		trafo3d.tex \
		kalib.tex \
		reko.tex \
		szdescr.tex \
		modescr.tex \
		matching.tex \
		hiddenln.tex \
		inspect.tex \
		robcntl.tex

ICEDOCPARTS=icedoc.tex $(ICESECT) $(IMAGES)

all:	pdf #	html

icedoc.proclist: $(ICEDOCPARTS) macros.tex
	-rm icedoc_fl_sorted.tex	
	-rm icedoc_fl_section.tex
	touch icedoc_fl_sorted.tex
	touch icedoc_fl_section.tex
	pdflatex icedoc.tex
	pdflatex icedoc.tex
	mv icedoc.glo icedoc.proclist

icedoc.pdf:  $(ICEDOCPARTS) macros.tex icedoc_fl_section.tex icedoc_fl_sorted.tex
	pdflatex icedoc.tex

icedoc.act:  $(ICEDOCPARTS) macros.tex icedoc_fl_section.tex icedoc_fl_sorted.tex icehtml.cfg
	mkdir -p html
	htlatex icedoc.tex "icehtml,index=2,3,frames," "" "-dhtml/"
#	htlatex icedoc.tex "icehtml,index=2,3" "" "-dhtml/"
	touch icedoc.act
	rm -f icedoc?.html
	rm -f icedocse*html
	rm -f icedocsu*html
#	rm -f *.4ct *.4tc *.aux *.dvi *.idv *.lg *.log *.tmp *.trc *.xref
	rm -f *.4es *.png tmp

icedoc_fl_section.tex icedoc_fl_sorted.tex: icedoc.proclist mkfunctionlist
	mkfunctionlist -2 -l icedoc proclist

pdf:   icedoc.pdf

html:  icedoc.act

mkfunctionlist: mkfunctionlist.ag mkfunctionlist.h mkfl_string.cpp
	AppGen mkfunctionlist.ag mkfunctionlist.cpp
	g++ mkfunctionlist.cpp mkfl_string.cpp -o mkfunctionlist
	rm mkfunctionlist.cpp

clean:		
		-rm -f *.dvi *.aux *.log *.glo *.ps *~ *.act *.toc
		-rm -f *.xref *eps-converted-to.pdf
		-rm -f icedoc_fl_*.tex
		touch robdoc.glo
		touch icedoc.glo
		-rm icedoc*html
		-rm icedoc*.png
		-rm icedoc.4ct icedoc.act icedoc.css icedoc.idv icedoc.log icedoc.xref
		-rm icedoc.4tc icedoc.lg icedoc.out  icedoc.proclist  icedoc.tmp
