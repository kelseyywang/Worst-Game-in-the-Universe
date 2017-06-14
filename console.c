#include "console.h"
#include "gl.h"
#include "font.h"
#include "malloc.h"
#include "printf.h"

int x_pos;
int y_pos;
char **buf;
const int MAX_ARRAY_SIZE = 1024;
int numCols;
int numRows;
int scrollIndex;
void drawAllChars();
void nullBuf();
void drawHelper(char ch, int swapEachTime);

//Initializes console with gl_init and allocates enough
//space in buffer to hold contents of whole screen
void console_init(unsigned nrows, unsigned ncols) {
	x_pos = 0;
	y_pos = 0;
	numCols = ncols;
	numRows = nrows;
	scrollIndex = 0;
	gl_init(ncols * font_get_width(), nrows * font_get_height(), 1);
	buf = (char **) malloc(sizeof(char *) * nrows);
	for(int r = 0; r < nrows; r++){
		buf[r] = (char *) malloc(sizeof(char) * ncols + 1);
	}
	nullBuf();
}

//Adds characters to screen using drawHelper
void console_putc(unsigned ch) {
	drawHelper((char) ch, 1);
}

//Sets all elements in buf to null terminator
void nullBuf() {
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			buf[i + scrollIndex][j] = '\0';
		}
	}
}

//Draws all previous and current characters on screen
void drawAllChars() {
	gl_clear(GL_BLACK);
	for (int y = 0; y <= numRows; y++) {
		for (int x = 0; x <= numCols; x++) {
			if (buf[y + scrollIndex][x] != '\0') {
				gl_draw_char(x * font_get_width(), y * font_get_height(), buf[y + scrollIndex][x], GL_BLUE);
			}
		}
	}
}

//Adds individual characters to screen, redrawing
//with all other characters and swapping buffer
void drawHelper(char ch, int swapEachTime) {
	if (x_pos >= numCols) {
		//Text wraparound
		x_pos = 0;
		y_pos++;
	}
	if (y_pos - scrollIndex >= numRows) {
		//Scroll - free up previous memory and allocate for new row
		free(buf[scrollIndex]);
		buf[y_pos] = (char *) malloc(sizeof(char) * numCols + 1);
		for (int i = 0; i <= numCols; i++) {
			buf[y_pos][i] = '\0';
		}
		scrollIndex++;
	}
	//Handles special cases
	if (ch == '\n') {
		y_pos++;
		x_pos = 0;
	}
	else if (ch == '\r') {
		x_pos = 0;
	}
	else if (ch == '\b') {
		if (x_pos == 0) {
			//Case when deleting from previous line
			x_pos = numCols - 1;
			y_pos--;
		}
		else {
			x_pos--;
		}
		if (y_pos >= 0 && x_pos > 0) {
			buf[y_pos][x_pos] = '\0';
		}
	}
	else if (ch == '\f') {
		nullBuf();
		x_pos = 0;
		y_pos = 0;
	}
	else {
		//Set normal character in buf
		buf[y_pos][x_pos] = ch;
		x_pos++;
		if (swapEachTime == 1) {
			drawAllChars();
		}
	}
	if (swapEachTime == 1) {
		gl_swap_buffer();
	}
}

//Prints string to screen with help from vsnprintf
//and drawHelper
int console_printf(const char *format, ...) {
	char s[MAX_ARRAY_SIZE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(s, MAX_ARRAY_SIZE, format, ap);

	int i = 0;
	while (s[i] != '\0') {
		drawHelper(s[i], 0);
		i++;
	}
	drawAllChars();
	gl_swap_buffer();
	return 0;
}

//Returns char at specified position
char console_get_char(const unsigned row, unsigned col) {
	return buf[row][col];
}
