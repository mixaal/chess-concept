CC=gcc
CFLAGS=-Wall
LIBS=
RM=rm -f

PROG=chess
OBJECTS=main.o board.o legal_moves.o chess_control.o

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LIBS)

clean:
	$(RM) $(PROG) $(OBJECTS)

