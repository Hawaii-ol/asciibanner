#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Windows.h>
#include "libbmp.h"
#define ferrorf(...) fprintf(stderr, __VA_ARGS__)

#define FONT_FILEPATH "C:\\Windows\\fonts\\msyh.ttc"
#define FONT_SIZE_PX 64
#define STR_MAX	512

int main()
{
	FT_Library		ftlib;
	FT_Face			face;
	FT_GlyphSlot	slot;
	FT_Error		error;
	char			str[STR_MAX];
	wchar_t* wstr;
	bmp_img			img;
	int				nwchar;
	int				pitch;
	int				pen_x = 0;
	int				pen_y = 0;
	int				baseline = 0;
	int				max_height = 0;
	int				total_width = 0;

	if (error = FT_Init_FreeType(&ftlib)) {
		ferrorf("Failed to initialize FreeType.\n");
		exit(EXIT_FAILURE);
	}

	if (error = FT_New_Face(ftlib, FONT_FILEPATH, 0, &face)) {
		ferrorf("Failed to read font file " FONT_FILEPATH ".\n");
		exit(EXIT_FAILURE);
	}
	slot = face->glyph;

	if (error = FT_Set_Pixel_Sizes(face, 0, FONT_SIZE_PX)) {
		ferrorf("Failed to set font size.\n");
		exit(EXIT_FAILURE);
	}

	fgets(str, STR_MAX, stdin);
	str[strcspn(str, "\n")] = '\0';
	nwchar = MultiByteToWideChar(GetConsoleCP(), 0, str, STR_MAX, NULL, 0);
	wstr = (wchar_t*)malloc(sizeof(wchar_t) * nwchar);
	if (MultiByteToWideChar(GetConsoleCP(), 0, str, STR_MAX, wstr, nwchar) == 0) {
		ferrorf("Failed to convert string encoding: %ld\n", (long)GetLastError());
		exit(EXIT_FAILURE);
	}

	// calculate position of baseline and total width
	for (int i = 0; wstr[i]; i++) {
		if (error = FT_Load_Char(face, wstr[i], FT_LOAD_RENDER))
			continue;
		if (slot->bitmap_top > baseline)
			baseline = slot->bitmap_top;
		total_width += (slot->advance.x >> 6) + 1;
	}

	// 50px margin on all four sides
	bmp_img_init_df(&img, total_width + 100, FONT_SIZE_PX + 100);
	pen_y = 50;
	// fill with white
	for (int i = 0; i < img.img_header.biHeight; i++) {
		for (int j = 0; j < img.img_header.biWidth; j++) {
			bmp_pixel_init(&img.img_pixels[i][j], 255, 255, 255);
		}
	}

	for (int i = 0; wstr[i]; i++) {
		if (error = FT_Load_Char(face, wstr[i], FT_LOAD_RENDER))
			continue;

		// locate pen, align along baseline
		pen_x = baseline - slot->bitmap_top + 50;

		pitch = slot->bitmap.pitch;
		for (int x = 0; x < slot->bitmap.rows; x++) {
			for (int y = 0; y < slot->bitmap.width; y++) {
				int show = slot->bitmap.buffer[x * pitch + y];
				if (show)
					bmp_pixel_init(&img.img_pixels[pen_x + x][pen_y + y], 0, 0, 0); // black color text
			}
		}
		pen_y += (slot->advance.x >> 6) + 1;
	}

	bmp_img_write(&img, "banner.bmp");
	bmp_img_free(&img);
	free(wstr);
	FT_Done_FreeType(ftlib);

	return 0;
}