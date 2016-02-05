tokenizer: tokenizer.c
	gcc -Wall -g -o tokenizer tokenizer.c 

alternate: tokenizer.c
	gcc -Wall -O -o tokenizer tokenizer.c

readme:
	texi2pdf readme.tex -q

package:
	cd .. && tar cfz Asst0.tgz Asst0/tokenizer.c Asst0/testcases.txt Asst0/readme.pdf

clean:
	rm -f tokenizer readme.aux readme.log readme.pdf
