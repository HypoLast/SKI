#ifndef __LEAKER_H__
#define __LEAKER_H__

#include <stdlib.h>
#define lfree(x) _lfree((void **)&(x))

void *lmalloc(size_t size);
void _lfree(void **ptr);
void lstat();

#endif