CC = gcc
CFLAGS = -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wno-long-long -std=c99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wdeclaration-after-statement -Wbad-function-cast -Wnested-externs -Winit-self -Wswitch-default -Wshadow -Wlogical-op -Wignored-qualifiers -O3
LDLIBS = -lm

%.out: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.c
	$(CC) -c $(CFLAGS) $<

all: main.out

main.out: main.o lodepng.o

main.o: main.c lodepng.h
lodepng.o: lodepng.c lodepng.h

clean:
	rm -f *.o *.out

.PHONY: clean

