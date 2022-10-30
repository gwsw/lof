APP=lof
DEBUG=0

CFLAGS = -Wall

ifeq ($(DEBUG),1)
LEXFLAGS = -d
CFLAGS += -DYYDEBUG
endif

YACC = bison
LEX = flex

OBJ = $(APP)c.o $(APP)w.o $(APP).lex.o expr.o list.o

$(APP): $(OBJ)
	$(CC) -o $(APP) $(CFLAGS) $(OBJ)

$(APP).yacc.c $(APP).yacc.h: $(APP).y
	$(YACC) -d -o $(APP).yacc.c $(APP).y

$(APP).lex.c: $(APP).l $(APP).yacc.h
	$(LEX) $(LEXFLAGS) $(APP).l && mv lex.yy.c $(APP).lex.c

$(APP)w.o: $(APP)w.c $(APP).yacc.c

clean:
	rm -f $(APP) $(APP).*.* *.o 
