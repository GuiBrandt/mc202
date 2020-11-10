CFLAGS = -std=c++11 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean

all: listas_legais

listas_legais: *.cpp
	g++ $(CFLAGS) *.cpp -o listas_legais $(LDFLAGS)

clean:
	rm -f listas_legais
	rm -f *.o
