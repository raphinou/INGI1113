#no -std=c99 for usint getopt: https://bugzilla.redhat.com/show_bug.cgi?id=130815
#CFLAGS+=-Wall -Werror -ansi -pedantic -std=c99
CFLAGS+=-Wall
LDFLAGS+=-lunrar -L. -lpthread
EXEC=rarcrack
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS) libunrar.so
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
libunrar.so:
	@echo "libunrar missing"
	@exit 1

.PHONY: clean mrproper

clean:
	@rm $(OBJECTS)

mrproper:
	@rm $(EXEC)
