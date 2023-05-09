all: a.out
	./a.out

a.out: parse.c
	gcc -g  -Wall -Wextra parse.c -fsanitize=address -fsanitize-recover=address -fstack-protector-all -fstack-check --param asan-stack=0 -Wextra -Werror

clean:
	rm -rf a.out
