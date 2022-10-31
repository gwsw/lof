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

static int usage() {
    printf("usage: lof [-d]\n");
    return 1;
}

static void handle_eval(char const* str) {
    Expr* expr = expr_from_string(str);
    if (expr == NULL)
        printf("invalid expression: %s\n", str);
    else
        printf("%d\n", expr_eval(expr));
}

static void handle_eq(char const* str1, char const* str2) {
    Expr* expr1 = expr_from_string(str1);
    if (expr1 == NULL)
        printf("invalid expression: %s\n", str1);
    else {
        Expr* expr2 = expr_from_string(str2);
        if (expr2 == NULL)
            printf("invalid expression: %s\n", str2);
        else {
            if (expr_eq(expr1, expr2))
                printf("equal\n");
            else
                printf("not equal\n");
            expr_free(expr2);
        }
        expr_free(expr1);
    }
}

static void handle_print(char* str) {
    Expr* expr = expr_from_string(str);
    if (expr == NULL)
        printf("invalid expression: %s\n", str);
    else {
        char rbuf[512];
        expr_to_string(expr, rbuf, sizeof(rbuf));
        expr_free(expr);
        printf(" %s\n", rbuf);
    }
}

static void handle_line(char* line) {
    if (*line == '?') {
        handle_eval(line+1);
    } else {
        char* eq = strchr(line, '=');
        if (eq != NULL) {
            *eq++ = '\0';
            handle_eq(line, eq);
        } else {
            handle_print(line);
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
        char line[512];
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        size_t len = strcspn(line, "\r\n");
        if (len == 0) continue;
        if (len >= sizeof(line)-1) {
            printf("line too long\n");
            continue;
        }
        line[len] = '\0';
        if (strcmp(line, "q") == 0) break;
        handle_line(line);
    }
    return 0;
}
