

#include <efi.h>
#include <efilib.h>
#include <bootloader.h>
#include <error.h>

CHAR16 error_message_buffer[256];



const CHAR16* get_efi_error_message(IN EFI_STATUS const status)
{
	
	StatusToString(error_message_buffer, status);
	return error_message_buffer;
}


bool check_for_fatal_error(IN EFI_STATUS const status,
	IN const CHAR16* error_message)
{
	if(EFI_ERROR(status)) {
		
		EFI_INPUT_KEY input_key;

		debug_print_line(L"Fatal Error: %s: %s\n", error_message,
			get_efi_error_message(status));

		#if PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR
			debug_print_line(L"Press any key to reboot...");
			wait_for_input(&input_key);
		#endif

		return TRUE;
	}

	return FALSE;
}
