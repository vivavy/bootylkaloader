

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <port_io.h>
#include <string.h>
#include <uart.h>


#define UART_PORT_COM1 0x3f8



void uart_initialize(void)
{
	outb(UART_PORT_COM1 + 1, 0x00);    
	outb(UART_PORT_COM1 + 3, 0x80);    
	outb(UART_PORT_COM1 + 0, 0x03);    
	outb(UART_PORT_COM1 + 1, 0x00);    
	outb(UART_PORT_COM1 + 3, 0x03);    
	outb(UART_PORT_COM1 + 2, 0xC7);    
	outb(UART_PORT_COM1 + 4, 0x0B);    
}



bool uart_is_recieve_buffer_empty(void)
{
	return inb(UART_PORT_COM1 + 5) & 1;
}



char uart_getchar(void)
{
	while(!uart_is_recieve_buffer_empty());

	return inb(UART_PORT_COM1);
}



bool uart_is_transmit_buffer_empty(void)
{
	return (inb(UART_PORT_COM1 + 5) & 0x20) != 0;
}



void uart_putchar(char a)
{
	while(!uart_is_transmit_buffer_empty());

	outb(UART_PORT_COM1, a);
}



void uart_puts(const char* str)
{
	
	size_t str_len = strlen(str);
	for(size_t i = 0; i < str_len; i++) {
		uart_putchar(str[i]);
	}
}
