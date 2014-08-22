

all: nanotrigger nanoexec


nanotrigger: nanotrigger.c Makefile
	gcc -O3 -o nanotrigger nanotrigger.c -lnanomsg

nanoexec: nanoexec.c Makefile
	gcc -O3 -o nanoexec nanoexec.c -lnanomsg



clean:
	rm nanotrigger nanoexec || true

indent:
	find . \( -name '*.c' -o -name '*.h' \) -exec indent  --braces-after-if-line --no-tabs  --k-and-r-style --line-length 90 --indent-level 4 -bli0 \{\} \;
