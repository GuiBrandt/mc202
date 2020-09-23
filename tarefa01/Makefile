CC=gcc
CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean memcheck

all: megasena

megasena: megasena.c
	$(CC) $(CFLAGS) megasena.c -o megasena $(LDFLAGS)

memcheck: megasena
	valgrind --leak-check=full ./megasena < ./testes/megasena1.in

profile: megasena
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes ./megasena < ./testes/megasena1.in
	callgrind_annotate --auto=yes

test:
	python3 testar.py megasena

clean:
	rm -f megasena
	rm -f callgrind.out.*
