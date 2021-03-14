project: proj.c error_handling.c
	gcc -Wall -Wextra -pedantic -g proj.c error_handling.c -o project

clean:
	rm project