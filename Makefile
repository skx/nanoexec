

all: trigger nanoexec


trigger: trigger.c Makefile
	gcc -O3 -o trigger trigger.c -lnanomsg

nanoexec: nanoexec.c Makefile
	gcc -O3 -o nanoexec nanoexec.c -lnanomsg


clean:
	rm trigger nanoexec || true

indent:
	find . \( -name '*.c' -o -name '*.h' \) -exec indent  --braces-after-if-line --no-tabs  --k-and-r-style --line-length 90 --indent-level 4 -bli0 \{\} \;
