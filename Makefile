CFLAG= -std=c99
all: SC.o main.o libcsv.a SC.h csv.h
	gcc -o sc SC.o libcsv.a main.o

.SUFFIXES: .o .c
.c.o:
	$(CC) -c $(CFLAG) -o $@ $<

clean:
	rm -rf *.o sc *_analysis

