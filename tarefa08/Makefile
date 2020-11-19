COPTFLAGS = -Ofast -DNDEBUG -march=native  -Winline -flto -freorder-blocks-and-partition -fstrict-aliasing -fwhole-program
CFLAGS = -std=c99 -Wall -Werror=vla -DNDEBUG
LDFLAGS = -lm

SOURCES = tree_multiset.c listas_legais.c
PROFILES = tree_multiset.gcda listas_legais.gcda

.PHONY: all clean debug release memcheck test coverage

all: listas_legais

listas_legais: $(PROFILES) $(SOURCES)
	$(CC) $(CFLAGS) $(COPTFLAGS) -fprofile-use $(SOURCES) -o listas_legais $(LDFLAGS)

debug: $(SOURCES)
	$(CC) $(CFLAGS) -g -ftest-coverage -fprofile-arcs $^ -o listas_legais $(LDFLAGS) -lgcov

clean:
	rm -f listas_legais
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda

memcheck: debug
	valgrind --leak-check=full ./listas_legais < ./testes/listas_legais8.in

$(PROFILES):
	$(CC) $(CFLAGS) $(COPTFLAGS) -fprofile-arcs $(SOURCES) -o listas_legais $(LDFLAGS)
	./listas_legais < ./testes/listas_legais10.in > /dev/null
	valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes --branch-sim=yes ./listas_legais < ./testes/listas_legais8.in > /dev/null
	callgrind_annotate --auto=yes

test: debug
	./listas_legais < ./testes/listas_legais10.in > ./testes/listas_legais10.out
	diff ./testes/listas_legais10.out ./testes/listas_legais10.res

coverage: debug test
	mkdir -p coverage
	gcovr -r . --xml -o coverage/coverage.xml
