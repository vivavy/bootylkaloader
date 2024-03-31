

#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include <stdint.h>


void draw_rect(uint32_t* framebuffer_pointer,
	const uint32_t pixels_per_scaline,
	const uint16_t _x,
	const uint16_t _y,
	const uint16_t width,
	const uint16_t height,
	const uint32_t color);

#endif
