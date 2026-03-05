CC = gcc
CFLAGS = -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wno-long-long -std=c99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wdeclaration-after-statement -Wbad-function-cast -Wnested-externs -O3
LDLIBS = -lm

%.out: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.c
	$(CC) -c $(CFLAGS) $<

all: main.out

main.out: main.o libmatc.o

main.o: main.c dijkstra_algorithm.h
libmatc.o: libmatc.c libmatc.h

clean:
	rm -f *.o *.out

.PHONY: clean

