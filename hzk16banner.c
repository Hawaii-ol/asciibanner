#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef unix
#include <iconv.h>
#include <string.h>
#endif
#ifdef WRITE_BMP
#include "libbmp.h"
#define PROGNAME "hzk16bmp"
#else
#define PROGNAME "hzk16ascii"
#endif
#define STR_MAX 512
#include "common.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	char text[STR_MAX] = {0};
	unsigned char* membitmap;
	int nchar = 0, ninvalid = 0;
	int pen_x, pen_y;
#ifdef WRITE_BMP
	bmp_img img;
	char outfile[STR_MAX];
	parse_args(argc, argv, text, STR_MAX, NULL, outfile, STR_MAX);
#else
	parse_args(argc, argv, text, STR_MAX, NULL, NULL, 0);
#endif

	if ((fp = fopen("HZK16", "rb")) == NULL) {
		perror("HZK16");
		return 1;
	}
	if (text[0] == '\0') {
		// read from stdin
		fgets(text, STR_MAX, stdin);
		text[strcspn(text, "\n")] = '\0';
	}
#ifdef unix
	{
		iconv_t cd;
		size_t inleft = STR_MAX, outleft = STR_MAX;
		char gbk[STR_MAX];
		char *inptr = text, *outptr = gbk;
		if ((cd = iconv_open("GBK", "UTF-8")) == (iconv_t)-1) {
			perror("iconv");
			return 1;
		}
		if (iconv(cd, &inptr, &inleft, &outptr, &outleft) == -1) {
			perror("iconv");
			return 1;
		}
		strncpy(text, gbk, STR_MAX);
	}
#endif
	// calculate total_width
	for (int i = 0; text[i]; i++) {
		unsigned char byte1, byte2;
		if (isascii(text[i])) {
			// halfwidth ascii to fullwidth GB2312
			byte1 = 0xA3;
			byte2 = text[i] + 0x80;
		}
		else {
			byte1 = text[i];
			byte2 = text[i + 1];
			i++;
		}
		// GB2312 encoding range 0xA1A1~0XF7FE
		if (byte1 < 0xA1 || byte1 > 0xF7 || byte2 < 0xA1 || byte2 > 0xFE) {
			ninvalid++;
			continue;
		}
		nchar++;
	}
	membitmap = (unsigned char*)malloc(nchar * 16 * 16);
	pen_y = 0;
#ifdef WRITE_BMP
	// 10px margin on all four sides
	bmp_img_init_df(&img, nchar * 16 + 20, 16 + 20);
	// fill with white
	for (int i = 0; i < img.img_header.biHeight; i++) {
		for (int j = 0; j < img.img_header.biWidth; j++) {
			bmp_pixel_init(&img.img_pixels[i][j], 255, 255, 255);
		}
	}
#endif

	for (int i = 0; text[i]; i++) {
		unsigned char byte1, byte2;
		if (isascii(text[i])) {
			byte1 = 0xA3;
			byte2 = text[i] + 0x80;
		}
		else {
			byte1 = text[i];
			byte2 = text[i + 1];
			i++;
		}
		if (byte1 < 0xA1 || byte1 > 0xF7 || byte2 < 0xA1 || byte2 > 0xFE)
			continue;

		fseek(fp, (94 * (byte1 - 0xA1) + (byte2 - 0xA1)) * 32, SEEK_SET);
		pen_x = 0;
		for (int j = 0; j < 32; j++) {
			int halfrow = fgetc(fp);
			for (int b = 0x80; b; b >>= 1) {
				membitmap[pen_x * nchar * 16 + pen_y] = !!(halfrow & b);
				pen_y++;
			}
			if (j & 1) {
				pen_x++;
				pen_y -= 16;
			}
		}
		pen_y += 16;
	}

#ifdef WRITE_BMP
	// write to bmp
	pen_x = pen_y = 10;
	for (int i = 0; i < nchar * 16 * 16; i++) {
		if (membitmap[i])
			bmp_pixel_init(&img.img_pixels[pen_x][pen_y], 0, 0, 0); // black color text
		pen_y++;
		if ((i + 1) % (nchar * 16) == 0) {
			pen_y = 10;
			pen_x++;
		}
	}
	bmp_img_write(&img, outfile);
	bmp_img_free(&img);
#else
	// write to stdout
	for (int i = 0; i < nchar * 16 * 16; i++) {
		putchar(membitmap[i] ? '*' : ' ');
		if ((i + 1) % (nchar * 16) == 0)
			putchar('\n');
	}
#endif
	if (ninvalid)
		printf("跳过了%d个不支持的字符\n", ninvalid);

	free(membitmap);
	return 0;
}