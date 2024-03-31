

#include <efi.h>
#include <efilib.h>
#include <stdarg.h>

#include <bootloader.h>
#include <error.h>
#include <serial.h>



EFI_STATUS configure_serial_protocol(IN EFI_SERIAL_IO_PROTOCOL* const protocol)
{
	
	EFI_STATUS status;

	#ifdef DEBUG
		debug_print_line(L"Debug: Configuring serial IO protocol\n");
	#endif

	status = uefi_call_wrapper(protocol->SetAttributes, 7,
		protocol, 0, 0, 0, 0, 0, DefaultStopBits);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Error: Error configuring Serial Protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	return EFI_SUCCESS;
}



EFI_STATUS init_serial_service(void)
{
	
	EFI_STATUS status;

	#ifdef DEBUG
		debug_print_line(L"Debug: Initialising Serial service\n");
	#endif

	status = uefi_call_wrapper(gBS->LocateProtocol, 3,
		&gEfiSerialIoProtocolGuid, NULL, &serial_service.protocol);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Error: Error locating Serial Protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Located Serial Protocol\n");
	#endif

	status = configure_serial_protocol(serial_service.protocol);
	if(EFI_ERROR(status)) {
		return status;
	}

	return EFI_SUCCESS;
}



EFI_STATUS print_to_serial_out(IN EFI_SERIAL_IO_PROTOCOL* const protocol,
	IN CHAR16* line)
{
	
	EFI_STATUS status;
	
	UINTN buffer_size = 0;
	
	UINTN line_length = 0;

	
	
	if(protocol == NULL ||
		line == NULL) {
		return EFI_INVALID_PARAMETER;
	}

	line_length = StrLen(line);

	
	if(line_length > MAX_SERIAL_OUT_STRING_LENGTH) {
		return EFI_BAD_BUFFER_SIZE;
	}

	
	if(line_length == 0) {
		return EFI_INVALID_PARAMETER;
	}

	
	
	
	
	buffer_size = line_length * 2;

	status = uefi_call_wrapper(protocol->Write, 3,
		protocol, &buffer_size, (VOID*)line);
	if(EFI_ERROR(status)) {
		
		debug_print_line(L"Error: Error writing to serial protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	
	if(buffer_size != (line_length * 2)) {
		debug_print_line(L"Error: Full string not printed to serial protocol\n");
		return EFI_DEVICE_ERROR;
	}

	return EFI_SUCCESS;
}
