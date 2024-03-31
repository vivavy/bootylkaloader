#ifndef HEX_H
#define HEX_H
#define identa (4)
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;
typedef struct {
    //todo: add magic
    unsigned char ident[identa];
    uint16_t version;
    uint16_t environment; // bare \ kernel ( 0x00 \ 0x01)
    uint64_t architechture; //architecture (x86 / arm)
    uint32_t bits; // bits (16 / 32 / 64)
    uint64_t type; //type (err / dummy / shared / executable)

} Hxe64_Hdr;
#endif  