project: proj.o error_handling.o
	gcc proj.o error_handling.o -o project

proj.o: proj.c
	gcc -Wall -Wextra -pedantic -g -c proj.c

error_handling.o: error_handling.c
	gcc -Wall -Wextra -pedantic -g -c error_handling.c

clean:
	rm project *.o