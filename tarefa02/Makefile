CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean memcheck profile test

all: professor_carlos

professor_carlos: professor_carlos_main.c professor_carlos_funcoes.c
	gcc $(CFLAGS) -c professor_carlos_funcoes.c $(LDFLAGS)
	gcc $(CFLAGS) -c professor_carlos_main.c $(LDFLAGS)
	gcc $(CFLAGS) -o professor_carlos professor_carlos_main.o professor_carlos_funcoes.o $(LDFLAGS)

memcheck: professor_carlos
	valgrind --leak-check=full ./professor_carlos < ./testes/tarefa02_15.in

profile: professor_carlos
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes ./professor_carlos < ./testes/tarefa02_15.in
	callgrind_annotate --auto=yes

test: professor_carlos
	python3 testar.py

clean:
	rm -f professor_carlos
	rm -f *.o
	rm -f callgrind.out.*