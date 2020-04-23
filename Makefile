program: program.c program-utils.c svd.c
	gcc -o program program.c program-utils.c svd.c -lm
clean:
	rm -f program program.o program-utils.o