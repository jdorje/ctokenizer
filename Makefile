tokenizer: tokenizer.c
	gcc -Wall -g -o tokenizer tokenizer.c 

readme:
	texi2pdf readme.tex -q

clean:
	rm -f tokenizer readme.aux readme.log readme.pdf
