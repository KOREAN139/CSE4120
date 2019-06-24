CC = gcc

CFLAGS = -g

OBJS = lex.yy.o tiny.tab.o main.o util.o symtab.o analyze.o

20151623: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o 20

main.o: main.c globals.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h globals.h
	$(CC) $(CFLAGS) -c analyze.c

tiny.tab.o: tiny.tab.c
	$(CC) $(CFLAGS) -c tiny.tab.c

lex.yy.c: tiny.l
	bison -d tiny.y
	flex tiny.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	-rm 20
	-rm $(OBJS)
	-rm lex.yy.c
	-rm tiny.tab*
