CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g -O2
LDFLAGS = -lm -lgcov

.PHONY: all clean

all: diadeuber

diadeuber: *.c
	gcc $(CFLAGS) *.c -o diadeuber $(LDFLAGS)

clean:
	rm -f diadeuber
	rm -f *.o
