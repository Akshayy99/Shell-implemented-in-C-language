makefil: parse.c builtin.c exec.c
	gcc -o makefil builtin.c parse.c exec.c -I.