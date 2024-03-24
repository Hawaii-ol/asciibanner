#include <stdio.h>
#include <locale.h>
#include "libbmp.h"
#ifdef unix
#include <iconv.h>
#include <string.h>
#endif
#define BUF_SIZE 256
#define _STR(x) #x
#define STR(x) _STR(x)

int main()
{
	FILE *fp;
	bmp_img img;
	unsigned char buf[BUF_SIZE] = {0};
	int nhanzi = 0, nother = 0;
	int pen_x = 0, pen_y = 0;

	if ((fp = fopen("HZK16", "rb")) == NULL) {
		perror("HZK16");
		return 1;
	}
	printf("输入一行汉字: ");
	scanf("%" STR(BUF_SIZE) "s", (char*)buf);
#ifdef unix
	{
		iconv_t cd;
		size_t inleft = BUF_SIZE, outleft = BUF_SIZE;
		unsigned char gbk[BUF_SIZE];
		char *inptr = (char*)buf, *outptr = (char*)gbk;
		if ((cd = iconv_open("GBK", "UTF-8")) == (iconv_t)-1) {
			perror("iconv");
			return 1;
		}
		if (iconv(cd, &inptr, &inleft, &outptr, &outleft) == -1) {
			perror("iconv");
			return 1;
		}
		strncpy((char*)buf, (char*)gbk, BUF_SIZE);
	}
#endif
	// count GB2312 hanzi
	for (int i = 0; buf[i]; i++) {
		// GB2312 encoding range 0xA1A1~0XFEFE
		if (buf[i] < 0xA1 || buf[i] > 0xFE || buf[i + 1] < 0xA1 || buf[i + 1] > 0xFE)
			continue;
		nhanzi++;
		i++;
	}
	// 10px margin on all four sides
	bmp_img_init_df(&img, nhanzi * 16 + 20, 16 + 20);
	pen_y = 10;
	// fill with white
	for (int i = 0; i < img.img_header.biHeight; i++) {
		for (int j = 0; j < img.img_header.biWidth; j++) {
			bmp_pixel_init(&img.img_pixels[i][j], 255, 255, 255);
		}
	}

	for (int i = 0; buf[i]; i++) {
		// GB2312 encoding range 0xA1A1~0XFEFE
		if (buf[i] < 0xA1 || buf[i] > 0xFE || buf[i + 1] < 0xA1 || buf[i + 1] > 0xFE) {
			nother++;
			// GBK character that does not exist in GB2312
			if (buf[i] >= 0x81 && buf[i] <= 0xFE && buf[i + 1] >= 0x40 && buf[i + 1] <= 0xFE)
				i++;
			continue;
		}
		fseek(fp, (94 * (buf[i] - 0xA1) + (buf[i + 1] - 0xA1)) * 32, SEEK_SET);
		pen_x = 10;
		for (int j = 0; j < 32; j++) {
			int halfrow = fgetc(fp);
			for (int b = 0x80; b; b >>= 1) {
				if (halfrow & b)
					bmp_pixel_init(&img.img_pixels[pen_x][pen_y], 0, 0, 0); // black color text
				pen_y++;
			}
			if (j & 1) {
				pen_x++;
				pen_y -= 16;
			}
		}
		pen_y += 16;
		i++;
	}
	if (nother)
		printf("跳过了%d个不支持的字符\n", nother);

	bmp_img_write(&img, "hzk16banner.bmp");
	bmp_img_free(&img);

	return 0;
}