
#include <efi.h>
#include <efilib.h>

#include <bootloader.h>
#include <error.h>
#include <graphics.h>

#define TEST_SCREEN_COL_NUM             4
#define TEST_SCREEN_ROW_NUM             3
#define TEST_SCREEN_TOTAL_TILES         TEST_SCREEN_COL_NUM * TEST_SCREEN_ROW_NUM
#define TEST_SCREEN_PRIMARY_COLOUR      0x00FF4000
#define TEST_SCREEN_SECONDARY_COLOUR    0x00FF80BF

EFI_STATUS find_video_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format,
	OUT UINTN* video_mode);



EFI_STATUS close_graphic_output_service()
{
	EFI_STATUS status = uefi_call_wrapper(gBS->FreePool, 1,
		graphics_serv.handle_buffer);

	return status;
}



VOID draw_rect(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT16 _x,
	IN const UINT16 _y,
	IN const 
 
 UINT16 width,
	IN const UINT16 height,
	IN const UINT32 color)
{
	
	UINT32* at;

	UINT16 row = 0;
	UINT16 col = 0;
	for(row = 0; row < height; row++) {
		for(col = 0; col < width; col++) {
			at = (UINT32*)protocol->Mode->FrameBufferBase + _x + col;
			at += ((_y + row) * protocol->Mode->Info->PixelsPerScanLine);

			*at = color;
		}
	}
}



VOID draw_test_screen(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol)
{
	const UINT16 tile_width = protocol->Mode->Info->HorizontalResolution /
		TEST_SCREEN_COL_NUM;
	const UINT16 tile_height = protocol->Mode->Info->VerticalResolution /
		TEST_SCREEN_ROW_NUM;

	UINT8 p = 0;
	for(p = 0; p < TEST_SCREEN_TOTAL_TILES; p++) {
		UINT8 _x = p % TEST_SCREEN_COL_NUM;
		UINT8 _y = p / TEST_SCREEN_COL_NUM;

		UINT32 color = TEST_SCREEN_PRIMARY_COLOUR;
		if(((_y % 2) + _x) % 2) {
			color = TEST_SCREEN_SECONDARY_COLOUR;
		}

		draw_rect(protocol, tile_width * _x, tile_height * _y,
			tile_width, tile_height, color);
	}
}



EFI_STATUS find_video_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format,
	OUT UINTN* video_mode)
{
	
	EFI_STATUS status;
	
	UINTN size_of_mode_info;
	
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;

	UINTN i = 0;
	for(i = 0; i < protocol->Mode->MaxMode; i++) {
		#ifdef DEBUG
			debug_print_line(L"Debug: Testing video mode: '%llu'\n", i);
		#endif

		status = uefi_call_wrapper(protocol->QueryMode, 4,
			protocol, i, &size_of_mode_info, &mode_info);
		if(EFI_ERROR(status)) {
			debug_print_line(L"Error: Error querying video mode: %s\n",
				get_efi_error_message(status));

			return status;
		}

		if(mode_info->HorizontalResolution == target_width &&
			mode_info->VerticalResolution == target_height &&
			mode_info->PixelFormat == target_pixel_format) {

			#ifdef DEBUG
				debug_print_line(L"Debug: Matched video mode: '%llu' for '%lu*%lu*%u'\n", i,
					target_width, target_height, target_pixel_format);
			#endif

			*video_mode = i;
			return EFI_SUCCESS;
		}
	}

	return EFI_UNSUPPORTED;
}



EFI_STATUS init_graphics_output_service(void)
{
	
	EFI_STATUS status;

	#ifdef DEBUG
		debug_print_line(L"Debug: Initialising Graphics Output Service\n");
	#endif

	
	status = uefi_call_wrapper(gBS->LocateHandleBuffer, 5,
		ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL,
		&graphics_serv.handle_count, &graphics_serv.handle_buffer);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Error: Error locating GOP handle buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		debug_print_line(L"Debug: Located GOP handle buffer with %u handles\n",
			graphics_serv.handle_count);
	#endif

	return EFI_SUCCESS;
}



EFI_STATUS set_graphics_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format)
{
	
	EFI_STATUS status;
	
	UINTN graphics_mode_num = 0;

	status = find_video_mode(protocol, target_width, target_height,
		target_pixel_format, &graphics_mode_num);
	if(EFI_ERROR(status)) {
		
		return status;
	}

	status = uefi_call_wrapper(protocol->SetMode, 2,
		protocol, graphics_mode_num);
	if(EFI_ERROR(status)) {
		debug_print_line(L"Error: Error setting graphics mode: %s\n",
			get_efi_error_message(status));

		return status;
	}

	return EFI_SUCCESS;
}
