
#include <efi.h>
#include <efilib.h>

#include <bootloader.h>
#include <error.h>
#include <fs.h>



EFI_STATUS init_file_system_service(void)
{
	#ifdef DEBUG
		debug_print_line(L"Debug: Initialising File System service\n");
	#endif

	EFI_STATUS status = uefi_call_wrapper(gBS->LocateProtocol, 3,
		&gEfiSimpleFileSystemProtocolGuid, NULL, &file_system_service.protocol);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Fatal Error: Error locating Simple File System Protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Located Simple File System Protocol\n");
	#endif

	return status;
}
