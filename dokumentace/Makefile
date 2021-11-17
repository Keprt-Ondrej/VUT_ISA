PROJEKT = manual

all:
	latex $(PROJEKT).tex
	bibtex $(PROJEKT)
	latex $(PROJEKT).tex
	latex $(PROJEKT).tex
	dvips -t a4 $(PROJEKT).dvi
	ps2pdf $(PROJEKT).ps
	mv $(PROJEKT).pdf ../

.PHONY: 
clean:
	rm -fr *.aux *.fdb_latexmk *.fls *.log *.gz $(PROJEKT).pdf *.zip *.dvi *.ps *.out *.blg *.bbl htm/ *toc