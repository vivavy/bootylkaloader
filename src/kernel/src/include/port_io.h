

#ifndef PORT_IO_H
#define PORT_IO_H

#include <stdint.h>


uint8_t inb(uint16_t port);


void outb(uint16_t port, uint8_t val);

#endif
