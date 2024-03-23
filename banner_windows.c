#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Windows.h>
#define ferrorf(...) fprintf(stderr, __VA_ARGS__)

#define FONT_FILEPATH "C:\\Windows\\fonts\\msyh.ttc"
#define FONT_SIZE_PX 24
#define STR_MAX	512

int main()
{
	FT_Library		ftlib;
	FT_Face			face;
	FT_GlyphSlot	slot;
	FT_Error		error;
	HANDLE			hstd;
	DWORD			conmode;
	char			str[STR_MAX];
	wchar_t*		wstr;
	int				nwchar;
	int				pitch;
	int				pen_x = 0;
	int				max_rows = 0;
	int				baseline = 0;
	
	if (error = FT_Init_FreeType(&ftlib)) {
		ferrorf("Failed to initialize FreeType.\n");
		exit(EXIT_FAILURE);
	}

	if (error = FT_New_Face(ftlib, FONT_FILEPATH, 0, &face)) {
		ferrorf("Failed to read font file.\n");
		exit(EXIT_FAILURE);
	}
	slot = face->glyph;
	
	if (error = FT_Set_Pixel_Sizes(face, 0, FONT_SIZE_PX)) {
		ferrorf("Failed to set font size.\n");
		exit(EXIT_FAILURE);
	}

	// enable virtual terminal sequence for conhost.exe
	hstd = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hstd, &conmode);
	SetConsoleMode(hstd, conmode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	system("cls");
	fgets(str, STR_MAX, stdin);
	str[strcspn(str, "\n")] = '\0';
	nwchar = MultiByteToWideChar(GetConsoleCP(), 0, str, STR_MAX, NULL, 0);
	wstr = (wchar_t*)malloc(sizeof(wchar_t) * nwchar);
	if (MultiByteToWideChar(GetConsoleCP(), 0, str, STR_MAX, wstr, nwchar) == 0) {
		ferrorf("Failed to convert string encoding: %ld\n", (long)GetLastError());
		exit(EXIT_FAILURE);
	}

	system("cls");
	// calculate position of baseline
	for (int i = 0; wstr[i]; i++) {
		if (error = FT_Load_Char(face, wstr[i], FT_LOAD_RENDER))
			continue;
		if (slot->bitmap_top > baseline)
			baseline = slot->bitmap_top;
	}

	for (int i = 0; wstr[i]; i++) {
		if (error = FT_Load_Char(face, wstr[i], FT_LOAD_RENDER))
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

	free(wstr);
	FT_Done_FreeType(ftlib);

 	return 0;
}