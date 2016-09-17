#include <stdlib.h>
#include <stdio.h>
#include "leaker.h"

int allocs = 0;
int allocSize = 0;
int frees = 0;

void *lmalloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr) {
		allocs ++;
		allocSize += size;
	}
	return ptr;
}

void _lfree(void **ptr) {
	if (*ptr) {
		frees ++;
	} else {
		fprintf(stderr, "\n ===LEAKER=== Attempt to free NULL\n");
	}
	free(*ptr);
	*ptr = NULL;
}

void lstat() {
	printf("Allocs: %d (%d bytes)\n", allocs, allocSize);
	printf("Frees:  %d\n", frees);
	printf("Leaks:  %d\n", allocs - frees);
}