#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

// User input buffer
static char input[2048];


int main(int argc, char** argv) {

	puts("Daffy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while(1) {
		char* input = readline("daffy> ");
		add_history(input);
		printf("%s\n", input);
		free(input);
	}
	return 0;
}
