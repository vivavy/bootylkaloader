

#include <efi.h>
#include <efilib.h>
#include <stdarg.h>

#include <bootloader.h>
#include <elf.h>
#include <error.h>
#include <fs.h>
#include <graphics.h>
#include <serial.h>

#define TARGET_SCREEN_WIDTH     1024
#define TARGET_SCREEN_HEIGHT    768
#define TARGET_PIXEL_FORMAT     PixelBlueGreenRedReserved8BitPerColor


Uefi_Graphics_Service graphics_serv;

Uefi_File_System_Service file_system_service;

Uefi_Serial_Service serial_service;


#define DRAW_TEST_SCREEN 0


EFI_STATUS get_memory_map(OUT VOID** memory_map,
	OUT UINTN* memory_map_size,
	OUT UINTN* memory_map_key,
	OUT UINTN* descriptor_size,
	OUT UINT32* descriptor_version);



EFI_STATUS debug_print_line(IN CHAR16* fmt,
	...)
{
	
	EFI_STATUS status;
	
	va_list args;
	
	CHAR16 output_message[MAX_SERIAL_OUT_STRING_LENGTH];

	va_start(args, fmt);

	
	
	if(serial_service.protocol) {
		VSPrint(output_message, MAX_SERIAL_OUT_STRING_LENGTH, fmt, args);

		status = print_to_serial_out(serial_service.protocol, output_message);
		if(EFI_ERROR(status)) {
			Print(L"Error: Error printing to serial output: %s\n",
				get_efi_error_message(status));

			return status;
		}
	} else {
		VPrint(fmt, args);
	}

	va_end(args);

	return EFI_SUCCESS;
};



EFI_STATUS get_memory_map(OUT VOID** memory_map,
	OUT UINTN* memory_map_size,
	OUT UINTN* memory_map_key,
	OUT UINTN* descriptor_size,
	OUT UINT32* descriptor_version)
{
	
	EFI_STATUS status;
	
	EFI_INPUT_KEY input_key;

	#ifdef DEBUG
		debug_print_line(L"Debug: Allocating memory map\n");
	#endif

	status = uefi_call_wrapper(gBS->GetMemoryMap, 5,
		memory_map_size, *memory_map, memory_map_key,
		descriptor_size, descriptor_version);
	if(EFI_ERROR(status)) {
		
		
		if(status != EFI_BUFFER_TOO_SMALL) {
			debug_print_line(L"Fatal Error: Error getting memory map size: %s\n",
				get_efi_error_message(status));

			#if PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR
				debug_print_line(L"Press any key to reboot...");
				wait_for_input(&input_key);
			#endif

			return status;
		}
	}

	
	*memory_map_size += 2 * (*descriptor_size);

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, *memory_map_size, (VOID**)memory_map);
	if(check_for_fatal_error(status, L"Error allocating memory map buffer")) {
		return status;
	}

	status = uefi_call_wrapper(gBS->GetMemoryMap, 5,
		memory_map_size, *memory_map, memory_map_key,
		descriptor_size, descriptor_version);
	if(check_for_fatal_error(status, L"Error getting memory map")) {
		return status;
	}

	return EFI_SUCCESS;
}



EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE* SystemTable)
{
	
	EFI_STATUS status;
	
	EFI_GRAPHICS_OUTPUT_PROTOCOL* graphics_output_protocol = NULL;
	
	EFI_FILE* root_file_system;
	
	EFI_PHYSICAL_ADDRESS* kernel_entry_point = 0;
	
	EFI_MEMORY_DESCRIPTOR* memory_map = NULL;
	
	UINTN memory_map_key = 0;
	
	UINTN memory_map_size = 0;
	
	UINTN descriptor_size;
	
	UINT32 descriptor_version;
	
	void (*kernel_entry)(Kernel_Boot_Info* boot_info);
	
	Kernel_Boot_Info boot_info;
	
	EFI_INPUT_KEY input_key;

	
	
	serial_service.protocol = NULL;
	file_system_service.protocol = NULL;

	
	InitializeLib(ImageHandle, SystemTable);

	
	status = uefi_call_wrapper(gBS->SetWatchdogTimer, 4, 0, 0, 0, NULL);
	if(check_for_fatal_error(status, L"Error setting watchdog timer")) {
		return status;
	}

	
	status = uefi_call_wrapper(ST->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
	if(check_for_fatal_error(status, L"Error resetting console input")) {
		return status;
	}
    
	
	status = init_serial_service();
	if(EFI_ERROR(status)) {
		if(status == EFI_NOT_FOUND) {
			#ifdef DEBUG
				debug_print_line(L"Debug: No serial device found\n");
			#endif
		} else {
			debug_print_line(L"Fatal Error: Error initialising Serial IO service\n");

			#if PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR
				debug_print_line(L"Press any key to reboot...");
				wait_for_input(&input_key);
			#endif

			return status;
		}
	}
    
	
	status = init_graphics_output_service();
	if(EFI_ERROR(status)) {
		if(status == EFI_NOT_FOUND) {
			#ifdef DEBUG
				debug_print_line(L"Debug: No graphics device found\n");
			#endif
		} else {
			debug_print_line(L"Fatal Error: Error initialising Graphics service\n");

			#if PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR
				debug_print_line(L"Press any key to reboot...");
				wait_for_input(&input_key);
			#endif

			return status;
		}
	}


	
	
	status = uefi_call_wrapper(gBS->OpenProtocol, 6,
		ST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid,
		&graphics_output_protocol, ImageHandle,
		NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Error: Failed to open the graphics output protocol on "
			L"the active console output device: %s\n", get_efi_error_message(status));
	}

	
	if(graphics_output_protocol) {
		status = set_graphics_mode(graphics_output_protocol, TARGET_SCREEN_WIDTH,
			TARGET_SCREEN_HEIGHT, TARGET_PIXEL_FORMAT);
		if(EFI_ERROR(status)) {
			
			return status;
		}

		#if DRAW_TEST_SCREEN != 0
			draw_test_screen(graphics_output_protocol);
		#endif
	}

	
	
	status = init_file_system_service();
	if(EFI_ERROR(status)) {
		
		return status;
	}

	status = uefi_call_wrapper(file_system_service.protocol->OpenVolume, 2,
		file_system_service.protocol, &root_file_system);
	if(check_for_fatal_error(status, L"Error opening root volume")) {
		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Loading Kernel image\n");
	#endif

	status = load_kernel_image(root_file_system, KERNEL_EXECUTABLE_PATH,
		kernel_entry_point);
	if(EFI_ERROR(status)) {
		
		
		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Set Kernel Entry Point to: '0x%llx'\n",
			*kernel_entry_point);
	#endif

	boot_info.video_mode_info.framebuffer_pointer =
		(VOID*)graphics_output_protocol->Mode->FrameBufferBase;
	boot_info.video_mode_info.horizontal_resolution =
		graphics_output_protocol->Mode->Info->HorizontalResolution;
	boot_info.video_mode_info.vertical_resolution =
		graphics_output_protocol->Mode->Info->VerticalResolution;
	boot_info.video_mode_info.pixels_per_scaline =
		graphics_output_protocol->Mode->Info->PixelsPerScanLine;

	#ifdef DEBUG
		debug_print_line(L"Debug: Closing Graphics Output Service handles\n");
	#endif

	status = close_graphic_output_service();
	if(check_for_fatal_error(status, L"Error closing Graphics Output service")) {
		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Getting memory map and exiting boot services\n");
	#endif

	
	status = get_memory_map((VOID**)&memory_map, &memory_map_size,
		&memory_map_key, &descriptor_size, &descriptor_version);
	if(EFI_ERROR(status)) {
		
		return status;
	}

	status = uefi_call_wrapper(gBS->ExitBootServices, 2,
		ImageHandle, memory_map_key);
	if(check_for_fatal_error(status, L"Error exiting boot services")) {
		return status;
	}

	
	boot_info.memory_map = memory_map;
	boot_info.memory_map_size = memory_map_size;
	boot_info.memory_map_descriptor_size = descriptor_size;

	
	kernel_entry = (void (*)(Kernel_Boot_Info*))*kernel_entry_point;
	
	kernel_entry(&boot_info);

	
	return EFI_LOAD_ERROR;
}



EFI_STATUS wait_for_input(OUT EFI_INPUT_KEY* key) {
	
	EFI_STATUS status;
	do {
		status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, key);
	} while(status == EFI_NOT_READY);

	return status;
}
