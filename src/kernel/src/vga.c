

#include <vga.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <uart.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;


static size_t vga_row;

static size_t vga_column;

static uint8_t vga_color;

static uint16_t* vga_buffer;



inline uint8_t create_vga_color_entry(enum vga_color fg,
	enum vga_color bg)
{
	return fg | bg << 4;
}



inline uint16_t create_vga_entry(unsigned char uc,
	uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}



void vga_initialize(void)
{
	vga_row = 0;
	vga_column = 0;
	vga_color = create_vga_color_entry(VGA_COLOR_LIGHT_GREY, VGA_COLOR_RED);
	vga_buffer = (uint16_t*)0xC03FF000;

	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = create_vga_entry(' ', vga_color);
		}
	}
	uart_puts("VGA: Initialised.\n");
}



void vga_set_color(uint8_t color)
{
	vga_color = color;
}



void vga_putchar(char c)
{
	
	const size_t index = vga_row * VGA_WIDTH + vga_column;
	vga_buffer[index] = create_vga_entry(c, vga_color);

	if(++vga_column == VGA_WIDTH) {
		vga_column = 0;

		if(++vga_row == VGA_HEIGHT) {
			vga_row = 0;
		}
	}
}



void vga_puts(const char* str)
{
	
	const size_t len = strlen(str);

	for (size_t i = 0; i < len; i++) {
		vga_putchar(str[i]);
	}
	uart_puts("VGA: Printed string.\n");	
}
