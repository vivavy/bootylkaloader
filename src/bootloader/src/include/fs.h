
#ifndef BOOTLOADER_FS_H
#define BOOTLOADER_FS_H 1

#include <efi.h>
#include <efilib.h>


typedef struct s_uefi_simple_file_system_service {
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* protocol;
} Uefi_File_System_Service;


EFI_STATUS init_file_system_service(void);

#endif
