CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Werror=vla -g -ftest-coverage -fprofile-arcs -O0
LDFLAGS=-lm -lgcov
COVFLAGS=--html --html-details -o coverage/index.html

.PHONY: all clean test memcheck profile

all: calculadora

calculadora: calculadora.o bignum.o
	$(CC) calculadora.o bignum.o -o calculadora $(LDFLAGS)

clean:
	rm -f *.o
	rm -f calculadora

memcheck: calculadora
	valgrind --leak-check=full --error-exitcode=1 ./calculadora < ./testes/calculadora11.in

profile: calculadora
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./calculadora < ./testes/calculadora11.in
	callgrind_annotate --auto=yes

test: calculadora
	python3 testar.py

coverage: calculadora test
	mkdir -p coverage
	gcovr -r . $(COVFLAGS)