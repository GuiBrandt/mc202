CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean

all: sala_de_espera

sala_de_espera: sala_de_espera.c deque.c
	gcc $(CFLAGS) sala_de_espera.c deque.c -o sala_de_espera $(LDFLAGS)

clean:
	rm -f sala_de_espera

memcheck: sala_de_espera
	valgrind --leak-check=full ./sala_de_espera < ./testes/sala_de_espera10.in

profile: sala_de_espera
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./sala_de_espera < ./testes/sala_de_espera10.in
	callgrind_annotate --auto=yes

test: sala_de_espera
	python3 testar.py

coverage: sala_de_espera test
	mkdir -p coverage
	gcovr -r . --xml -o coverage/coverage.xml