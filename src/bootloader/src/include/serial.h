
#ifndef BOOTLOADER_SERIAL_H
#define BOOTLOADER_SERIAL_H 1

#include <efi.h>
#include <efilib.h>


#define MAX_SERIAL_OUT_STRING_LENGTH 256

typedef struct s_uefi_serial_service {
	EFI_SERIAL_IO_PROTOCOL* protocol;
} Uefi_Serial_Service;


EFI_STATUS configure_serial_protocol(IN EFI_SERIAL_IO_PROTOCOL* const protocol);


EFI_STATUS init_serial_service(void);


EFI_STATUS print_to_serial_out(IN EFI_SERIAL_IO_PROTOCOL* const protocol,
	IN CHAR16* line);

#endif
