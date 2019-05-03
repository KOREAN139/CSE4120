CC = gcc

CFLAGS =

OBJS = lex.yy.o tiny.tab.o main.o util.o

20151623: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o 20151623

main.o: main.c globals.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

tiny.tab.o: tiny.tab.c
	$(CC) $(CFLAGS) -c tiny.tab.c

lex.yy.c: tiny.l
	bison -d tiny.y
	flex tiny.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	-rm 20151623
	-rm $(OBJS)
	-rm lex.yy.c
	-rm tiny.tab*
