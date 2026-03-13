CC = gcc
CFLAGS = -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wno-long-long -std=c99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wdeclaration-after-statement -Wbad-function-cast -Wnested-externs -Winit-self -Wswitch-default -Wshadow -Wlogical-op -Wignored-qualifiers -O3
LODEPNG_FLAGS = -DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS -DLODEPNG_NO_COMPILE_ERROR_TEXT -DLODEPNG_NO_COMPILE_CPP
LDLIBS = -lm

%.out: %.o
	$(CC) -o $@ $^ $(LDLIBS)
%.o: %.c
	$(CC) -c $(CFLAGS) $<

all: main.out

main.out: main.o lodepng.o ocean_traffic.o

main.o: main.c ocean_traffic.h
ocean_traffic.o: ocean_traffic.c ocean_traffic.h lodepng.h
lodepng.o: lodepng.c lodepng.h
	$(CC) -c $(CFLAGS) $(LODEPNG_FLAGS) $<

clean:
	find . -maxdepth 1 -name "*.png" ! -name "source.png" -delete
	rm -f *.o *.out


.PHONY: clean

