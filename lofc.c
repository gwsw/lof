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

static char* concat(char** strings, unsigned num_strings) {
    int size = 1;
    for (unsigned i = 0; i < num_strings; ++i)
        size += strlen(strings[i]);
    char* r = malloc(size);
    r[0] = '\0';
    for (unsigned i = 0; i < num_strings; ++i)
        strcat(r, strings[i]);
    return r;
}

static int parse_fd(FILE* fd) {
    yyset_in(fd);
    return yyparse();
}

int main(int argc, char* argv[]) {
    int ch;
    int istring = 0;
#ifdef YYDEBUG
    yydebug = 0;
#endif
    while ((ch = getopt(argc, argv, "ds")) != -1) {
        switch (ch) {
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#else
            printf("not compiled with YYDEBUG\n");
#endif
            break;
        case 's':
            istring = 1;
            break;
        default:
            return usage();
        }
    }
#ifdef YYDEBUG
    yyset_debug(yydebug);
#endif
    int ret = 0;
    if (istring) {
        char* str = concat(argv+optind, argc-optind);
        ExprList* el = expr_list_from_string(str);
        char rbuf[512];
        expr_list_to_string(el, rbuf, sizeof(rbuf));
        expr_list_free(el);
        free(str);
        printf("ROUND-TRIP(%s)\n", rbuf);
    } else if (optind == argc) {
        if (parse_fd(stdin)) ret = 1;
    } else {
        while (optind < argc) {
            char const* file = argv[optind++];
            FILE* f = fopen(file, "r");
            if (f == NULL) {
                printf("cannot open %s\n", file);
                ret = 1;
            } else {
                if (parse_fd(f)) ret = 1;
            }
        }
    }
    return ret;
}
