CFLAGS = -std=c99 -Wall -Werror=vla -Winline -Ofast -DNDEBUG -march=native -flto -freorder-blocks-and-partition -fstrict-aliasing -fweb -fwhole-program
LDFLAGS = -lm -flto

.PHONY: all clean

all: listas_legais

listas_legais: tree_multiset.o listas_legais.o
	$(CC) $(CFLAGS) $^ -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
