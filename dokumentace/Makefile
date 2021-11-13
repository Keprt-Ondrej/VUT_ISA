PROJEKT = manual

all:
	#latex $(PROJEKT).tex
	#dvips -t a4 $(PROJEKT).dvi
	#ps2pdf $(PROJEKT).ps
	
	pdflatex $(PROJEKT).tex
	mv $(PROJEKT).pdf ../

.PHONY: 
clean:
	rm -rf *.dvi *.ps *.pdf *.aux *.log *fls *out *xmk *gz *toc html/