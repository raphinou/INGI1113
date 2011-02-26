CFLAGS+=-Wall -Werror -ansi -pedantic -I.
LDFLAGS+=-lpthread
EXEC=zipcrack
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
bnd_buf_testing: $(OBJECTS)
	$(CC) $(CFLAGS) -o bnd_buf_testing tests/bnd_buf_testing.c bnd_buf.o
.PHONY: clean mrproper

clean:
	@rm $(OBJECTS)

mrproper:
	@rm $(EXEC)
