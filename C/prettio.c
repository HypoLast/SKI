#include <stdlib.h>
#include <stdio.h>
#include "prettio.h"
#include "getch.h"

void left();
void right();
void up();
void down();

char *history[100];
int historyIdx = 0;
int histRoll = 0;
char *promptStr;
int scanBack = 0;

const char UP_ARROW = 0x41;
const char DOWN_ARROW = 0x42;
const char RIGHT_ARROW = 0x43;
const char LEFT_ARROW = 0x44;

void prettio_initialize(int lineSize, char *prompt) {
	promptStr = prompt;
}

void getLine(char *buf) {
	char ch;
	int i = 0;
	buf[0] = '\0';
	printf(promptStr);
	fflush(stdout);
	while((ch = getch()) != '\n') {
		if (ch == 0x7f) { // backspace
			if (i == 0) continue;
			i --;
			buf[i] = '\0';
		} else if (ch == 0x1b) { // arrow control
			getch(); // next char is 0x5b
			switch(getch()) { // which arrow was pressed?
				case UP_ARROW:
				case DOWN_ARROW: break; // TODO: history
				case RIGHT_ARROW: {
					if (scanBack > 0) scanBack --;
					break;
				}
				case LEFT_ARROW: {
					if (scanBack < i) scanBack ++;
				}
			}
		} else { // I guess it's a regular character?
			int j;
			for (j = i; j > i - scanBack; j --) {
				buf[j] = buf[j - 1];
			}
			buf[i - scanBack] = ch;
			i ++;
			buf[i] = '\0';
		}
		printf("\33[2K\r");
		printf("%s%s", promptStr, buf);
		
		fflush(stdout);
	}
	printf("\n");
}

void prettio_cleanup() {

}

void left() {
	putchar(0x1b);
	putchar(0x5b);
	putchar(LEFT_ARROW);
}

void right() {
	putchar(0x1b);
	putchar(0x5b);
	putchar(RIGHT_ARROW);
}

void up() {
	putchar(0x1b);
	putchar(0x5b);
	putchar(UP_ARROW);
}

void down() {
	putchar(0x1b);
	putchar(0x5b);
	putchar(DOWN_ARROW);
}