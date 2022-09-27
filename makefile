CC=gcc
CFLAGS=-Wall -I.
DEPS=constants.h commandhelper.h
OBJS=main.o commandhelper.o
EXE=shell
DFLAGS=-g

shell: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(CFLAGS)

main.o: main.c $(DEPS)
	$(CC) -c main.c $(CFLAGS)

commandhelper.o: commandhelper.c $(DEPS)
	$(CC) -c commandhelper.c $(CFLAGS)

run:
	./$(EXE)

debug: $(DEPS)
	$(CC) $(DFLAGS) -o shell main.c commandhelper.c commandhelper.h $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(EXE)