#include "gl.h"
#include "fb.h"
#include "font.h"
#include "printf.h"

/* BASIC GRAPHICS LIBRARY */
//Initializes GL with depth 4
void gl_init(unsigned width, unsigned height, unsigned mode) {
  printf("GL INITED");
  fb_init(width, height, 4, mode);
}

//Swaps buffer
void gl_swap_buffer() {
  fb_swap_buffer();
}

//Returns int representation of color with specified r, g, b
color gl_color(unsigned char r, unsigned char g, unsigned char b) {
  unsigned int retColor;
  retColor = (0xff << 24) | (r << 16) | (g << 8) | b;
  return retColor;
}

//Sets entire screen to color c
void gl_clear(color c) {
  for (int i = 0; i < fb_get_width(); i++) {
    for (int j = 0; j < fb_get_height(); j++) {
      gl_draw_pixel(i, j, c);
    }
  }
}

//Draws pixel at specified location with color c
void gl_draw_pixel(int x, int y, color c) {
  unsigned width_pixels = fb_get_pitch() * 8 / fb_get_depth();
  unsigned (*pixel)[width_pixels];
  if (x >= width_pixels || y >= fb_get_height()) {
    return;
  }
  else {
    pixel = (unsigned (*)[width_pixels])fb_get_draw_buffer();
  }
  pixel[y][x] = c;
}

//Returns color of pixel at location
color gl_read_pixel(int x, int y) {
  unsigned width_pixels = fb_get_pitch() * 8 / fb_get_depth();
  unsigned (*pixel)[width_pixels] = (unsigned (*)[width_pixels])fb_get_draw_buffer();
  color c = pixel[y][x];
  return c;
}

//Draws rectangle with specified coordinates and dimensions
void gl_draw_rect(int x, int y, int w, int h, color c) {
  unsigned width_pixels = fb_get_pitch() * 8 / fb_get_depth();
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      if (i + x >= width_pixels || j + y >= fb_get_height()) {
        break;
      }
      gl_draw_pixel(i + x, j + y, c);
    }
  }
}

//Draws character at location with color c, calls font_get_char
//on temporary font buffer and copies to our buffer
void gl_draw_char(int x, int y, char ch, color c) {
  char temp[font_get_size()];
  font_get_char(ch, temp, font_get_size());
  color (*colorTemp)[font_get_height()][font_get_width()] = (color (*)[font_get_height()][font_get_width()]) temp;
  for (int x_font = 0; x_font < font_get_width(); x_font++) {
    for (int y_font = 0; y_font < font_get_height(); y_font++) {
      if ((*colorTemp)[y_font][x_font]  == 0xFFFFFFFF) {
        gl_draw_pixel(x_font + x, y_font + y, c);
      }
    }
  }
}

//Calls gl_draw_char for entire string
void gl_draw_string(int x, int y, char* str, color c) {
  char *currChar = str;
  int currX = x;
  while (*currChar != '\0') {
    gl_draw_char(currX, y, *currChar, c);
    currX += font_get_width();
    currChar++;
  }
}
