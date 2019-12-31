CC=gcc

all: inject extract

inject: inject.c
	$(CC) -o $@ $<

extract: extract.c
	$(CC) -o $@ $<

clean:
	rm -f inject extract