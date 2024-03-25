#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Windows.h>
#include "libbmp.h"

#define FONT_FILEPATH "C:\\Windows\\fonts\\msyh.ttc"
#ifdef WRITE_BMP
#include "libbmp.h"
#define FONT_SIZE_PX 64
#define PROGNAME "bmpbanner"
#else
#define FONT_SIZE_PX 24
#define PROGNAME "banner"
#endif
#define STR_MAX	512
#include "common.h"

int main(int argc, char *argv[])
{
	FT_Library		ftlib;
	FT_Face			face;
	FT_GlyphSlot	slot;
	FT_Error		error;
	char			text[STR_MAX];
	wchar_t*		wtext;
	char*			membitmap;
	int				font_size;
	int				nwchar;
	int				pitch;
	int				pen_x = 0;
	int				pen_y = 0;
	int				baseline = 0;
	int				max_height = 0;
	int				total_width = 0;
#ifdef WRITE_BMP
	bmp_img			img;
	char			outfile[STR_MAX];
	parse_args(argc, argv, text, STR_MAX, &font_size, outfile, STR_MAX);
#else
	parse_args(argc, argv, text, STR_MAX, &font_size, NULL, 0);
#endif

	if (error = FT_Init_FreeType(&ftlib)) {
		errorf("Failed to initialize FreeType.\n");
		exit(EXIT_FAILURE);
	}

	if (error = FT_New_Face(ftlib, FONT_FILEPATH, 0, &face)) {
		errorf("Failed to read font file \"" FONT_FILEPATH "\".\n");
		exit(EXIT_FAILURE);
	}
	slot = face->glyph;

	if (error = FT_Set_Pixel_Sizes(face, 0, font_size)) {
		errorf("Failed to set font size.\n");
		exit(EXIT_FAILURE);
	}

	if (text[0] == '\0') {
		// read from stdin
		fgets(text, STR_MAX, stdin);
		text[strcspn(text, "\n")] = '\0';
	}
	nwchar = MultiByteToWideChar(GetConsoleCP(), 0, text, STR_MAX, NULL, 0);
	wtext = (wchar_t*)malloc(sizeof(wchar_t) * nwchar);
	if (MultiByteToWideChar(GetConsoleCP(), 0, text, STR_MAX, wtext, nwchar) == 0) {
		errorf("Failed to convert string encoding: %ld\n", (long)GetLastError());
		exit(EXIT_FAILURE);
	}

	// calculate position of baseline, total width and max_height
	for (int i = 0; wtext[i]; i++) {
		if (error = FT_Load_Char(face, wtext[i], FT_LOAD_RENDER))
			continue;
		if (slot->bitmap_top > baseline)
			baseline = slot->bitmap_top;
		total_width += slot->advance.x >> 6;
	}
	max_height = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
	membitmap = (unsigned char*)calloc(total_width, max_height);
	pen_y = 0;
#ifdef WRITE_BMP
	// 10px margin on all four sides
	bmp_img_init_df(&img, total_width + 20, max_height + 20);
	// fill with white
	for (int i = 0; i < img.img_header.biHeight; i++) {
		for (int j = 0; j < img.img_header.biWidth; j++) {
			bmp_pixel_init(&img.img_pixels[i][j], 255, 255, 255);
		}
	}
#endif

	for (int i = 0; wtext[i]; i++) {
		if (error = FT_Load_Char(face, wtext[i], FT_LOAD_RENDER))
			continue;

		// locate pen, align along baseline
		pen_x = baseline - slot->bitmap_top;
		pitch = slot->bitmap.pitch;
		for (int x = 0; x < slot->bitmap.rows; x++) {
			for (int y = 0; y < slot->bitmap.width; y++) {
				membitmap[(pen_x + x) * total_width + (pen_y + y) + slot->bitmap_left] = slot->bitmap.buffer[x * pitch + y];
			}
		}
		pen_y += slot->advance.x >> 6;
	}

#ifdef WRITE_BMP
	// write to bmp
	for (int x = 0; x < max_height; x++) {
		for (int y = 0; y < total_width; y++) {
			if (membitmap[x * total_width + y])
				bmp_pixel_init(&img.img_pixels[x + 10][y + 10], 0, 0, 0); // black color text
		}
	}
	bmp_img_write(&img, outfile);
	bmp_img_free(&img);
#else
	// write to stdout
	for (int x = 0; x < font_size; x++) {
		for (int y = 0; y < total_width; y++) {
			putchar(membitmap[x * total_width + y] ? '*' : ' ');
		}
		putchar('\n');
	}
#endif
	free(membitmap);
	free(wtext);
	FT_Done_FreeType(ftlib);
	return 0;
}