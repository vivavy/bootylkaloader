
#ifndef BOOTLOADER_ERROR_H
#define BOOTLOADER_ERROR_H 1

#include <efi.h>
#include <efilib.h>
#include <stdbool.h>


const CHAR16* get_efi_error_message(IN EFI_STATUS const status);


bool check_for_fatal_error(IN EFI_STATUS const status,
	IN const CHAR16* error_message);

#endif
