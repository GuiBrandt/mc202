CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm

.PHONY: all clean

all: corretor

hash_def.h: hash_def.h.tpl gen_hash.py
	python3 gen_hash.py > hash_def.h

corretor: *.c
	gcc $(CFLAGS) *.c -o corretor $(LDFLAGS)

clean:
	rm -f corretor hash_def.h
