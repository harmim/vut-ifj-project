# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

DOC = dokumentace

INC_DIR = inc


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
		$(INC_DIR)/*-eps-converted-to* $(DOC).toc
