CFLAGS = -std=c99 -Wall -Werror=vla -Winline -Ofast -DNDEBUG -march=native -flto -freorder-blocks-and-partition -fstrict-aliasing -fweb -fwhole-program --param large-function-growth=100000000 --param inline-unit-growth=1000
LDFLAGS = -lm -flto

.PHONY: all clean

all: listas_legais

listas_legais: tree_multiset.c listas_legais.c
	$(CC) $(CFLAGS) $^ -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
