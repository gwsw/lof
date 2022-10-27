APP=lof
DEBUG=0

CFLAGS = -Wall

ifeq ($(DEBUG),1)
LEXFLAGS = -d
CFLAGS += -DYYDEBUG
endif

XOBJ = expr.o

$(APP): $(APP)c.o $(APP)w.o $(APP).lex.o $(XOBJ)
	$(CC) -o $(APP) $(APP)c.o $(APP)w.o $(APP).lex.o $(XOBJ)

$(APP).yacc.c $(APP).yacc.h: $(APP).y
	bison -d -o $(APP).yacc.c $(APP).y

$(APP).lex.c: $(APP).l $(APP).yacc.h
	lex $(LEXFLAGS) $(APP).l && mv lex.yy.c $(APP).lex.c

$(APP)w.o: $(APP)w.c $(APP).yacc.c

clean:
	rm -f $(APP) $(APP).*.* *.o 
