CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean

all: fakenews

fakenews: fakenews.c
	gcc $(CFLAGS) fakenews.c -o fakenews $(LDFLAGS)

memcheck: fakenews
	valgrind --leak-check=full ./fakenews < ./testes/fakenews15.in

profile: fakenews
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./fakenews < ./testes/fakenews15.in
	callgrind_annotate --auto=yes

test: fakenews
	python3 testar.py

clean:
	rm -f fakenews
