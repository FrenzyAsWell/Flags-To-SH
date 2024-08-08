CC=gcc
CFLAGS= -lncurses

SRCDIR= ../src
EXEC=flags-to-sh

all: $(EXEC)

$(EXEC): $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -o flags-to-sh $(SRCDIR)/main.c

clean:
	rm -r $(EXEC)
