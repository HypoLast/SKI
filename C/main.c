#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SKIlib.h"
#include "leaker.h"
#include "prettio.h"
#include "getch.h"

char prompt[3] = "> ";

int main(int argc, char* argv[]) {
	getch();
	getch();
	printf("%x\n", getch());
	getch();
	getch();
	printf("%x\n", getch());
	getch();
	getch();
	printf("%x\n", getch());
	getch();
	getch();
	printf("%x\n", getch());

	// printf("SK(I)\n");
	// char buffer[4096];
	// prettio_initialize(4096, prompt);
	// getLine(buffer);
	// printf("%s\n", buffer);


	// execute("S`KSKfgx", (ExecOpts){ 0, -1 });
	// printf("\n");

	// ExecOpts opts;
	// opts.step = 0;
	// opts.maxIterations = 0;
	// for(;;) {
		// printf("> ");
		// fgets(buffer, 4096, stdin);
		// printf("%x %x %x\n", buffer[0], buffer[1], buffer[2]);
		// printf("%d\n", strcmp(buffer, ":q"));
		// if (!strcmp(buffer, ":q")) break;
		// execute(buffer, opts);
	// }


	lstat();
	return 0;
}