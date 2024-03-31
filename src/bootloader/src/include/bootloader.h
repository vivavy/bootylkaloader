
#ifndef BOOTLOADER_H
#define BOOTLOADER_H 1

#include <efi.h>
#include <efilib.h>
#include <cfg.h>
#include <fs.h>
#include <graphics.h>
#include <serial.h>


#define KERNEL_EXECUTABLE_PATH L"kernel.elf"


#define PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR 1

typedef struct s_boot_video_info {
	VOID* framebuffer_pointer;
	UINT32 horizontal_resolution;
	UINT32 vertical_resolution;
	UINT32 pixels_per_scaline;
} Kernel_Boot_Video_Mode_Info;

typedef struct s_boot_info {
	EFI_MEMORY_DESCRIPTOR* memory_map;
	UINTN memory_map_size;
	UINTN memory_map_descriptor_size;
	Kernel_Boot_Video_Mode_Info video_mode_info;
} Kernel_Boot_Info;

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE* SystemTable);

EFI_STATUS load_kernel_image(IN EFI_FILE* const root_file_system,
	IN CHAR16* const kernel_image_filename,
	OUT EFI_PHYSICAL_ADDRESS* kernel_entry_point);


EFI_STATUS wait_for_input(EFI_INPUT_KEY* key);

EFI_STATUS debug_print_line(IN CHAR16* fmt,
	...);

extern Uefi_Graphics_Service graphics_serv;

extern Uefi_File_System_Service file_system_service;

extern Uefi_Serial_Service serial_service;

#endif
