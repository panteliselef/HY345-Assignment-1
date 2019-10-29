cs345sh: cs345sh.o my_utils.o
	gcc cs345sh.o my_utils.o -o cs345sh.out
cs345sh.o: cs345sh.c 
	gcc -c -ansi -pedantic -Wall cs345sh.c
my_utils.o: my_utils.c my_utils.h
	gcc -c -ansi -pedantic -Wall my_utils.c

clean:
	@echo ‘Cleaning files...’
	@rm -f *.o *.out