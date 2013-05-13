# tips from Lincoln Mullen
# http://lincolnmullen.com 

PDFS := $(patsubst src/%.md,%.md.pdf,$(wildcard src/*.md))
PARAMS := --include-in-header=header.tex
all : $(PDFS)

%.md.pdf : src/%.md
	pandoc $< $(PARAMS) -o $@

clean :
	rm $(PDFS)

rebuild : clean all
