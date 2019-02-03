CC=gcc
CFLAGS=-Wall
LIBS=
RM=rm -f

PROG=board
OBJECTS=board.o legal_moves.o

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LIBS)

clean:
	$(RM) $(PROG) $(OBJECTS)

