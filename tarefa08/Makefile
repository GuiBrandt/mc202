CFLAGS = -std=c99 -Wall -Werror -Werror=vla -O2
LDFLAGS = -lm

.PHONY: all clean memcheck test coverage

all: listas_legais

listas_legais: tree_multiset.c listas_legais.c
	$(CC) $(CFLAGS) $^ -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda

memcheck: listas_legais
	valgrind --leak-check=full ./listas_legais < ./testes/listas_legais8.in

test: listas_legais
	python3 testar.py

coverage: test
	mkdir -p coverage
	gcovr -r . --xml -o coverage/coverage.xml
