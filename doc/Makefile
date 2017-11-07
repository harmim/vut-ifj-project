# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

DOC = dokumentace

IMG_DIR = img


$(DOC).pdf: $(DOC).tex
	pdflatex $<
	make bibtex
	pdflatex $<
	pdflatex $<


.PHONY: bibtex
bibtex: $(DOC).bib
	bibtex $(DOC)


.PHONY: clean
clean:
	rm -f $(DOC).aux $(DOC).dvi $(DOC).log $(DOC).ps $(DOC).synctex.gz \
		$(DOC).fls $(DOC).fdb_latexmk $(DOC).bbl $(DOC).blg $(DOC).out \
		$(IMG_DIR)/*-eps-converted-to* $(DOC).toc
