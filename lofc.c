#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char* argv[]) {
    int ch;
	int sset = 0;
#ifdef YYDEBUG
    yydebug = 0;
#endif
    while ((ch = getopt(argc, argv, "ds:")) != -1) {
        switch (ch) {
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#else
            printf("not compiled with YYDEBUG\n");
#endif
            break;
		case 's':
			sset = 1;
			break;
        default:
            return usage();
        }
    }
#ifdef YYDEBUG
    yyset_debug(yydebug);
#endif
    int ret = 0;
	if (sset) {
		char* str = concat(argv+optind, argc-optind);
		FILE* str_fd = fmemopen(str, strlen(str), "r");
		yyset_in(str_fd);
		if (yyparse()) ret = 1;
		fclose(str_fd);
		free(str);
	} else if (optind == argc) {
		yyset_in(stdin);
		if (yyparse()) ret = 1;
	} else {
		while (optind < argc) {
			char const* file = argv[optind++];
			FILE* f = fopen(file, "r");
			if (f == NULL) {
				printf("cannot open %s\n", file);
				ret = 1;
			} else {
				yyset_in(f);
				if (yyparse()) ret = 1;
			}
		}
	}
	return ret;
}
