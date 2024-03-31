		

#ifndef BOOTLOADER_GRAPHICS_H
#define BOOTLOADER_GRAPHICS_H 1

#include <efi.h>
#include <efilib.h>


typedef struct s_uefi_ {
	EFI_HANDLE* handle_buffer;
	UINTN handle_count;
} Uefi_Graphics_Service;

VOID draw_rect(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT16 _x,
	IN const UINT16 _y,
	IN const UINT16 _width,
	IN const UINT16 _height,
	IN const UINT32 color);


VOID draw_test_screen(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol);

EFI_STATUS close_graphic_output_service(void);


EFI_STATUS init_graphics_output_service(void);


EFI_STATUS set_graphics_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format);

#endif
