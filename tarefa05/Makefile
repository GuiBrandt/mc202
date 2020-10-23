CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS=-lm

.PHONY: all clean

all: calculadora

calculadora: calculadora.c bignum.c
	$(CC) $(CFLAGS) calculadora.c bignum.c -o calculadora $(LDFLAGS)

clean:
	rm -f calculadora

memcheck: calculadora
	valgrind --leak-check=full ./calculadora < ./testes/calculadora11.in

profile: calculadora
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./calculadora < ./testes/calculadora11.in
	callgrind_annotate --auto=yes

test: calculadora
	python3 testar.py
