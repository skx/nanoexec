

all: trigger nanoexec


trigger: trigger.c Makefile
	gcc -O3 -o trigger trigger.c -lnanomsg

nanoexec: nanoexec.c Makefile
	gcc -O3 -o nanoexec nanoexec.c -lnanomsg


clean:
	rm trigger nanoexec || true