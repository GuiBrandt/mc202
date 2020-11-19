CFLAGS = -std=c99 -Wall -Werror=vla -Winline -Ofast -DNDEBUG -march=native -flto -freorder-blocks-and-partition -fstrict-aliasing -fwhole-program
LDFLAGS = -lm -flto

.PHONY: all clean

all: listas_legais

listas_legais: tree_multiset.c listas_legais.c
	$(CC) $(CFLAGS) $^ -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
