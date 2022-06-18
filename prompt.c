#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

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
#endif

long eval_op(long x, char* op, long y) {
	if(strcmp(op, "+") == 0 || strcmp(op, "add") == 0) { return x + y; }
	if(strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) { return x - y; }
	if(strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) { return x * y; }
	if(strcmp(op, "/") == 0 || strcmp(op, "div") == 0) { return x / y; }
	if(strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) { return x % y; }
	if(strcmp(op, "^") == 0 || strcmp(op, "exp") == 0) { return pow(x, y); }
	if(strcmp(op, "min") == 0) { return x <= y ? x : y; }
	return 0;
}

long eval(mpc_ast_t* t) {

	// return directly if number
	if(strstr(t->tag, "number")) {
		return atoi(t->contents);
	}

	// Operator is always second child
	char* op = t->children[1]->contents;

	// Store the third child in x
	long x = eval(t->children[2]);

	// Iterate through the remaining children and combining
	int i = 3;
	while (strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}

int num_leaves(mpc_ast_t* t) {
	printf("node contents: %s\n\n", t->contents);
	if (t->children_num == 0) {
		return 1;
	}
	if (t->children_num >= 1) {
		int total = 0;
    		for (int i = 0; i < t->children_num; i++) {
      			total = total + num_leaves(t->children[i]);
  		}
		return total;
	}
	return 0;
}

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
	operator : '+' | '-' | '*' | '/' | '%' | '^' |\"add\" | \"exp\" | \"mul\" | \"sub\" | \"div\" | \"mod\" | \"exp\" | \"min\";  \
	expr     : <number> | '(' <operator> <expr>+ ')' ;  \
	lispy    : /^/ <operator> <expr>+ /$/ ;             \
	",
	Number, Operator, Expr, Lispy);

	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while(1) {

		char* input = readline("lispy> ");
		add_history(input);

		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			// print the AST if it succeeds
                        long result = eval(r.output);
                        printf("%li\n", result);
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
