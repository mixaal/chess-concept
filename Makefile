CC=gcc
CFLAGS=-Wall -Wstrict-prototypes -ggdb
LIBS=
RM=rm -f

PROG=chess
OBJECTS=board.o legal_moves.o chess_control.o fitness.o

all: $(PROG)

$(PROG): main.o $(OBJECTS)
	$(CC) -o $@ main.o $(OBJECTS) $(LIBS)

test: test.o test_helper.o $(OBJECTS)
	$(CC) -o $@ test.o test_helper.o $(OBJECTS)
	./test

clean:
	$(RM) $(PROG) $(OBJECTS) main.o test.o test test_helper.o

