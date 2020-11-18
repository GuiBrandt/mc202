CFLAGS = -std=c99 -Wall -Werror=vla -O3
LDFLAGS = -lm

.PHONY: all clean

all: listas_legais

listas_legais: *.c
	$(CC) $(CFLAGS) *.c -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
