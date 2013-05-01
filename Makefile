CFLAG= -std=c99
all: SC.o main.o libcsv.o SC.h csv.h
	gcc -o sc SC.o libcsv.o main.o

.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAG) -c -o $@ $<

clean:
	rm -rf *.o sc *_analysis
sort: add_analysis sub_analysis  mul_analysis
	sort -t ',' -n -k 6 add_analysis -o add_analysis
	sort -t ',' -n -k 6 mul_analysis -o mul_analysis
	sort -t ',' -n -k 6 sub_analysis -o sub_analysis
