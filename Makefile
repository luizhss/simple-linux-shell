all: shell

shell: builtin.o utils.o data_structures.o functions.h shell.c 
	gcc -o shell builtin.o utils.o data_structures.o shell.c

builtin.o: builtin.c functions.h
		gcc -Wall -c builtin.c

utils.o: utils.c functions.h
		gcc -Wall -c utils.c

data_structures.o: data_structures.c functions.h
		gcc -Wall -c data_structures.c

clean:
	rm -f shell builtin.o shell data_structures.o utils.o
	rm -f core *~