# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

DOC = dokumentace

IMG_DIR = img


$(DOC).pdf: $(DOC).ps
	ps2pdf -sPAPERSIZE=a4 $<


$(DOC).ps: $(DOC).dvi
	dvips $<


$(DOC).dvi: $(DOC).tex
	latex $<
	latex $<


.PHONY: pdflatex
pdflatex: $(DOC).tex
	pdflatex $<
	pdflatex $<


.PHONY: clean
clean:
	rm -f $(DOC).aux $(DOC).dvi $(DOC).log $(DOC).ps $(DOC).synctex.gz \
		$(DOC).fls $(DOC).fdb_latexmk $(DOC).bbl $(DOC).blg $(DOC).out \
		$(IMG_DIR)/*-eps-converted-to*
