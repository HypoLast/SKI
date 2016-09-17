#ifndef __PRETTIO_H__
#define __PRETTIO_H__

void prettio_initialize(int lineSize, char *prompt);
void getLine(char *buf);
void prettio_cleanup();

#endif