CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g  -ftest-coverage -fprofile-arcs -O0
LDFLAGS = -lm -lgcov

.PHONY: all clean

all: mensageiro

mensageiro: mensageiro.o tokens.o
	gcc $(CFLAGS) $^ -o mensageiro $(LDFLAGS)

clean:
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda
	rm -f mensageiro

memcheck: mensageiro
	valgrind --leak-check=full ./mensageiro < ./testes/mensageiro10.in

profile: mensageiro
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./mensageiro < ./testes/mensageiro10.in
	callgrind_annotate --auto=yes

test: mensageiro
	python3 testar.py

coverage: mensageiro test
	mkdir -p coverage
	gcovr -r . --xml -o coverage/coverage.xml