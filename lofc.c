#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "expr.h"

#ifdef YYDEBUG
extern int yydebug;
extern void yyset_debug(int);
#endif

extern int yyparse(void);
extern void yyset_in(FILE*);
extern int line_num;

static int usage() {
    printf("usage: lang [file...]\n");
    return 1;
}

void yyerror(const char* str) {
    fprintf(stderr, "ERROR: line %d: %s\n", line_num, str);
}

int yywrap() {
    return 1;
}

static void handle_line(char* line) {
    if (*line == '?') {
        Expr* expr = expr_from_string(line+1);
        if (expr == NULL)
            printf("invalid expression\n");
        else
            printf("%d\n", expr_eval(expr));
    } else {
        char* eq = strchr(line, '=');
        if (eq == NULL) {
            Expr* expr = expr_from_string(line);
            if (expr == NULL)
                printf("invalid expression\n");
            else {
                char rbuf[512];
                expr_to_string(expr, rbuf, sizeof(rbuf));
                expr_free(expr);
                printf("ROUND-TRIP(%s)\n", rbuf);
            }
        } else {
            *eq++ = '\0';
            Expr* expr1 = expr_from_string(line);
            Expr* expr2 = expr_from_string(eq);
            if (expr1 == NULL || expr2 == NULL)
                printf("invalid expression\n");
            else {
                int value1 = expr_eval(expr1);
                int value2 = expr_eval(expr2);
                if (value1 == value2)
                    printf("equal (%d)\n", value1);
                else
                    printf("not equal (%d != %d)\n",  value1, value2);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int ch;
#ifdef YYDEBUG
    yydebug = 0;
#endif
    while ((ch = getopt(argc, argv, "d")) != -1) {
        switch (ch) {
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#else
            printf("not compiled with YYDEBUG\n");
#endif
            break;
        default:
            return usage();
        }
    }
#ifdef YYDEBUG
    yyset_debug(yydebug);
#endif

    for (;;) {
        printf(">");
        char line[256];
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        size_t len = strcspn(line, "\r\n");
        line[len] = '\0';
        if (strcmp(line, "q") == 0) break;
        handle_line(line);
    }
    return 0;
}
