CFLAGS=-std=c99 -Wall -Werror -Werror=vla -g -O2
LDFLAGS=-lm

.PHONY: all clean

all: diadeuber

diadeuber: *.c
	gcc $(CFLAGS) *.c -o diadeuber $(LDFLAGS)

clean:
	rm -f diadeuber
	rm -f *.o

memcheck: diadeuber
	valgrind --leak-check=full ./diadeuber < ./testes/diadeuber10.in

profile: diadeuber
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./diadeuber < ./testes/diadeuber10.in
	callgrind_annotate --auto=yes

test: diadeuber
	python3 testar.py

coverage: diadeuber test
	mkdir -p coverage
	gcovr -r . --xml -o coverage/coverage.xml
