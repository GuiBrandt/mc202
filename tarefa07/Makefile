CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean

all: mensageiro

mensageiro: mensageiro.o tokens.o
	gcc $(CFLAGS) $^ -o mensageiro $(LDFLAGS)

clean:
	rm -f mensageiro
