#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Windows.h>

#define FONT_FILEPATH "C:\\Windows\\fonts\\msyh.ttc"
#define FONT_SIZE_PX 24
#define STR_MAX	512
#define PROGNAME "banner"
#include "common.h"

int main(int argc, char* argv[])
{
	FT_Library		ftlib;
	FT_Face			face;
	FT_GlyphSlot	slot;
	FT_Error		error;
	HANDLE			hstd;
	DWORD			conmode;
	char			text[STR_MAX];
	wchar_t*		wtext;
	int				font_size;
	int				nwchar;
	int				pitch;
	int				pen_x = 0;
	int				baseline = 0;
	int				max_rows = 0;

	parse_args(argc, argv, text, STR_MAX, &font_size, NULL, 0);

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

	// enable virtual terminal sequence for conhost.exe
	hstd = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hstd, &conmode);
	SetConsoleMode(hstd, conmode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

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

	system("cls");
	// calculate position of baseline and total width
	for (int i = 0; wtext[i]; i++) {
		if (error = FT_Load_Char(face, wtext[i], FT_LOAD_RENDER))
			continue;
		if (slot->bitmap_top > baseline)
			baseline = slot->bitmap_top;
	}

	for (int i = 0; wtext[i]; i++) {
		if (error = FT_Load_Char(face, wtext[i], FT_LOAD_RENDER))
			continue;

		// locate pen, align along baseline
		printf("\033[%d;%dH", baseline - slot->bitmap_top + 1, pen_x);
		if (baseline - slot->bitmap_top + slot->bitmap.rows > max_rows)
			max_rows = baseline - slot->bitmap_top + slot->bitmap.rows;
		
		pitch = slot->bitmap.pitch;
		for (int x = 0; x < slot->bitmap.rows; x++) {
			for (int y = 0; y < slot->bitmap.width; y++) {
				int show = slot->bitmap.buffer[x * pitch + y];
				putchar(show ? '*' : ' ');
			}
			printf("\033[%dD", slot->bitmap.width);
			printf("\033[1B");
		}
		pen_x += (slot->advance.x >> 6) + 1;
	}
	printf("\033[%dd", max_rows + 1);

	free(wtext);
	FT_Done_FreeType(ftlib);

 	return 0;
}