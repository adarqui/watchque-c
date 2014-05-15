all:
	gcc -ggdb -g3 -I include/ -Wall -O3 watch.c parse.c dump.c misc.c inotify.c redis.c main.c bucket.c list.c -o watchque -lev -lhiredis

clean:
	rm -f watchque
