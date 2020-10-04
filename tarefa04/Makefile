CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean memcheck profile test

all: seaofwords

seaofwords: seaofwords.c
	gcc $(CFLAGS) seaofwords.c -o seaofwords $(LDFLAGS)

memcheck: seaofwords
	valgrind --leak-check=full ./seaofwords < ./testes/seaofwords10.in

profile: seaofwords
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./seaofwords < ./testes/seaofwords10.in
	callgrind_annotate --auto=yes

test: seaofwords
	python3 testar.py

clean:
	rm -f seaofwords
