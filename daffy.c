#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

char* readline(char*);
void add_history(char*);
long eval_op(long, char*, long);
long eval(mpc_ast_t*);

typedef struct mpc_ast_t() {
	char* tag;
	char* contents;
	mpc_state_t state;
	int children_num;
	struct mpc_ast_t** children;  
} mpc_ast_t;

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {

	/* making the parsers */
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr     = mpc_new("expr");
	mpc_parser_t* Lispy    = mpc_new("lispy");

	/* define the grammar */
	mpca_lang(MPCA_LANG_DEFAULT,
	"                                                     \
	number   : /-?[0-9]+/ ;                             \
	operator : '+' | '-' | '*' | '/' | '%' | '^' | '.' | \"add\" | \"exp\" | \"mul\" | \"sub\" | \"div\" | \"mod\" ;  \
	expr     : <number> | '(' <operator> <expr>+ ')' ;  \
	lispy    : /^/ <operator> <expr>+ /$/ ;             \
	",
	Number, Operator, Expr, Lispy);

	puts("Daffy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while(1) {

		char* input = readline("daffy> ");
		add_history(input);

		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			// print the AST if it succeeds
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else {
			// print the error upon failure
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);
	}

	mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}
